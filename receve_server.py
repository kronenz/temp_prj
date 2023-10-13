from fastapi import FastAPI, Body
import datetime
from elasticsearch import Elasticsearch
import uvicorn

app = FastAPI()

es_host = '127.0.0.1'
es_port = '9200'
es_index = 'temp_results'

# Initialize Elasticsearch client
es = Elasticsearch([{
    'host': es_host,
    'port': es_port
}])

def initElasticsearchClient():
 
    # Check connection
    if es.ping():
        print("Connected to Elasticsearch!")
    else:
        print("Failed to connect to Elasticsearch.")

@app.post("/api/temp/5sec")
async def receive_post_request(device_ip: str = Body(...),device_nm: str = Body(...), humidity: float = Body(...), temperature_celsius: float = Body(...), temperature_fahrenheit: float = Body(...)):
    """Arduino 온도 센서에서 전송된 POST 요청을 처리하는 함수입니다."""
    
    # 서울의 시간대 정보를 가져옵니다.
    timezone_seoul = datetime.timezone(datetime.timedelta(hours=9))

    # 현재 시간을 서울 시간으로 변환합니다.
    now_seoul = datetime.datetime.now(timezone_seoul)

    # 타임스탬프를 출력합니다.
    timestamp = now_seoul.timestamp()
    
    #습도, 섭씨, 화씨
    print(f"POST 요청 수신 ({timestamp}): device = {device_nm}, humidity={humidity}, temperature_celsius={temperature_celsius}, temperature_fahrenheit={temperature_fahrenheit}")

    #elastic search document create
    document = {
        "timestamp" : timestamp,
        "device_ip" : device_ip,
        "device_nm" : device_nm,
        "humidity" : humidity,
        "temperature_celsius" : temperature_celsius,
        "temperature_fahrenheit" : temperature_fahrenheit
    }

    index_result = es.index(index=es_index, doc_type="_doc", body=document)
    if index_result['result'] == 'created':
        print('Document indexed successfully.')
        print(document)
    else:
        print('Failed to index document.')
        
    return {"message": "ok"}

if __name__ == "__main__":

    initElasticsearchClient()
    uvicorn.run(app, host="192.168.0.2", port=8000)


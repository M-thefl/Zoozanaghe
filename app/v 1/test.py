import requests

payloads = [
    {}, 
    {"test": "value"},  
    {
        "system_name": "S",
        "pc_name": "MyPC",
        "os_info": "Windows 10"
    }
]

for payload in payloads:
    response = requests.post(
        'http://127.0.0.1:3000/save_data',
        headers={'Content-Type': 'application/json'},
        json=payload
    )
    print(f"Payload: {payload} → Status: {response.status_code}, Response: {response.text}")
    print(response.headers)

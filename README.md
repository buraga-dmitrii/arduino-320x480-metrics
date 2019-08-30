# arduino-320x480-metrics
Display live metrics from parsed json


Required JSON:   

```json
{
    "metrics": {
        "avg_db": 15,
        "avg_duration": 17,
        "max_db": 1367,
        "max_duration": 1530,
        "statuses": {
            "200": 6124,
            "406": 1,
            "404": 29,
            "302": 1,
            "all": 6156,
            "409": 1,
            "grouped": {
                "2xx": 6124,
                "4xx": 31,
                "3xx": 1
            }
        }
    }
}
```

Result:   

![](https://i.imgur.com/QBDNHwo.jpg)

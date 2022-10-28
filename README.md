# Transport Catalogue
Транспортный справочник

## Описание
Транспортный справочник - это приложение, которое оперирует базой данных остановок и маршрутов и позволяет получать статистические данные, строить карту маршрутов и находить кратчайший маршрут.

Любые запросы к транспортному справочнику можно передать через JSON-файл. Для заполнения базы данных транспортного справочника используются запросы base_requests. Для получения данных из транспортного справочника используются запросы stat_requests. Кроме того, для настройки параметров карты используется запрос render_settings, а для настройки параметров движения транспорта - запрос routing_settings.

Запросы **base_requests** на заполнение базы данных бывают двух типов:
  * "Bus" - информация о маршруте; состоит из названия, списка остановок, типа маршрута (кольцевой/некольцевой).
    
    Пример запроса:
```
        {
            "is_roundtrip": false,
            "name": "35",
            "stops": [
                "Gastello",
                "TRK Piter"
            ],
            "type": "Bus"
        }
```

  * "Stop" - информация об остановке; состоит из названия, gps-координат, реального расстояния до других остановок. 

    Пример запроса:
```
        {
            "latitude": 55.555871,
            "longitude": 37.69087,
            "name": "Gastello",
            "road_distances": {
                 "TRK Piter": 1500
            },
            "type": "Stop"
        }
```    

Запросы **stat_requests** на получение данных бывают следующих типов:
  * "Bus" - информация о маршруте

    Пример запроса:
```
        {
            "id": 2,
            "name": "35",
            "type": "Bus"
        }
```



    Пример ответа:
    
```
        {
            "curvature": 0.796523,
            "request_id": 1,
            "route_length": 10180,
            "stop_count": 6,
            "unique_stop_count": 5
        }       
```        

Картинка:

![alt text](https://github.com/fediukov/ya_transport_catalogue/blob/main/map_example.png "Map Example")


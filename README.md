weather-aqi-dashboard

Qt-based desktop application that displays real-time weather, daily forecasts, and air quality (AQI) in a clean, dashboard-style interface. Built using C++ and Qt Widgets, the app features a visually polished UI with custom graphics, gauges, icons, and intuitive navigation.

<br><br>




Weather Dashboard
<img width="1917" height="983" alt="image" src="https://github.com/user-attachments/assets/ddb09953-090e-4024-835a-35a323f9fe68" />


                Current temperature
                Weather condition (clear sky, clouds, rain, etc.) 
                Humidity
                Atmospheric pressure
                Wind speed
                Visibility
                Dynamic weather icons
                Date & digital clock display
                Sunset/Sunrise
                Min/Max Temperature  


<br><br>

            

      

Daily Weather Dashboard
<img width="1915" height="978" alt="image" src="https://github.com/user-attachments/assets/d174f80c-5661-424d-8b3f-ff9ac66a483b" />

5-Day / 3-Hour Forecast
              
                Each day has a dedicated button. (QButtonGroup)
                Clicking a day:
                    Filters forecast to show only that date
                    Populates 8 forecast widgets (3-hour intervals)
                    Updates "Feels Like vs Actual" chart for that day only
                    Automatically handles incomplete days (sometimes 3–6 entries)
                Dynamic Charts
                    Actual Temperature vs Feels-Like (line chart)
                    Min/Max Temperature comparison
                    Auto-generates X-axis labels based on time
                    Smooth anti-aliased rendering


<br><br>




Air Quality Dashboard

https://github.com/user-attachments/assets/1a1611cb-cbac-4536-9120-e89093ba8bb1

                Real-time AQI value
                Main pollutant (e.g., PM2.5)
                Multi-color AQI gauge
                AQI scale with categories (Good → Hazardous)
                Smooth UI and animated components

<br><br>





API Services Used  
This application uses two separate API services:
                
                Weather Data - OpenWeatherMap API
                Air Quality Index  - IQAir AirVisual API, retrieves Air Quality Index (AQI) data using the AirVisual “Nearest City” endpoint.

Why Two APIs?
Using two specialized APIs ensures:

               More accurate weather data 
               More detailed and precise AQI (via a dedicated air quality API)
               Better pollutant breakdown
               More reliability when one API has rate limiting or downtime


Application Logic (Weather + AQI)

        Weather Logic Flow
              User enters a city
              App requests weather + forecast from OWM
              Parses values: temperature, wind, temperature, coordinates, etc
              Forecast list is filtered by selected day
              UI updates widgets + charts

        AQI Logic Flow

              App sends lat-lon data to AQI API: Get Coordinates from Weather API
              These coordinates are then passed into the AirVisual API.
              API returns pollutant concentrations
              App converts values to:
                 AQI value (US scale)
                 AQI color category
                 UI updates AQI indicator widget
                 Dominant pollutant
                 Health indicator (Good, Moderate, Unhealthy, etc.)
                        







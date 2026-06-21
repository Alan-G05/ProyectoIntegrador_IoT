<h1 style="background-color: #c1c1c1; text-align: center">Sistema de Iluminación que responde a diferentes Géneros Musicales</h1>



<div style="text-align: justify">

Proyecto final que presenta desarrollo de un sistema IoT que clasifica géneros musicales mediante Machine Learning y sincroniza la iluminación LED en tiempo real.

Integrantes:
- Cristian Mendez Hernandez
- González López Alan Antonio
- Ramírez Teutle José Guadalupe

</div>



## Arquitectura del Sistema

<div style="text-align: justify">

* **ML (Modelo):** Random Forest para clasificación de audio (GTZAN dataset).
* **Procesamiento:** Raspberry Pi 3B+. Actúa como cliente MPD para detectar la canción en reproducción y ejecuta el modelo de ML en tiempo real.
* **Protocolo:** Comunicación vía MQTT (Mosquitto MQTT Broker). 
* **Actuador:** ESP32 con tiras NeoPixel (WS2812). Suscriptor MQTT que interpreta el ID del género y ejecuta la animación LED correspondiente.
</div>


## Estructura del Repositorio
<div style="text-align: justify">

* **/ModeloLM**: Entrenamiento y exportación del modelo.
* **/RaspberryPi**: Script de inferencia y lógica de MPD.
* **/ESP32**: Código fuente para la tira de LEDs.
</div>

## Justificación del Proyecto como Sistema IoT
<div style="text-align: justify">

Este proyecto se clasifica como Internet de las Cosas (IoT) debido a que integra una arquitectura de hardware distribuido compuesta por nodos inteligentes que interactúan de forma autónoma a través de una red local. La Raspberry Pi funciona como el nodo de procesamiento de borde (Edge Computing), encargada de la analítica de audio en tiempo real y la toma de decisiones basada en Inteligencia Artificial. La comunicación entre los componentes se realiza mediante el protocolo estándar de mensajería MQTT (Message Queuing Telemetry Transport), el cual permite una arquitectura de publicación/suscripción eficiente y de baja latencia entre el motor de inferencia y el nodo actuador (ESP32). La capacidad del sistema para monitorizar el estado de reproducción musical, procesar señales complejas en el borde y ejecutar una respuesta física tangible en un entorno remoto, cumpliendo con los estándares de conectividad y reactividad, es lo que define a esta solución como una implementación de IoT orientada a la domótica inteligente.
</div>
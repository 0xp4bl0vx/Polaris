# Polaris

Polaris es un dispositivo IoT formado por un ESP8266 que monitoriza temperaturas. Cuenta con
una aplicacion web que sirve para la administraciñon de multiples dispositivos y la visualización
de las mediciones. Cuenta con alertas automáticas y configuración via WiFi.

# Desarrollo
El proyecto aun esta en desarrollo. Este es el esquema actual del dispositivo:

![Circuit](circuit.png)

El formulario de configuracion y el código actual del arduino ya estan disponibles. El código del arduino
esta practicamente terminado a falta de probarlo en busca de bugs y añadir un par de funcionalidades más.
La aplicacion web esta en fase de desarrollo y actualizare el repositorio con la primera versión de la API.         

El programa cambiar modo sirve para poner el dispositivo en modo configuración, en un futuro esta función 
estara implementada en un botón. Por otro lado, el CSS de la página de configuración no esta terminado y 
puede no formar parte de la versión final por optimizar el uso de la memoria.

Conforme el proyecto avance actualizare el esquema y este README con las nuevas características.

# Licencia
Este proyecto esta bajo la licencia de Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0) license.
Más info aqui: https://creativecommons.org/licenses/by-nc-sa/4.0/

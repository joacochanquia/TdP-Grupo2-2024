# TdP-Grupo2-2024
## Plataforma Dibujante con Reconocimiento de Imágenes

Este proyecto consiste en un robot capaz de dibujar figuras en una hoja de papel mediante el control de motores paso a paso. Además, incorpora un sistema de visión por computadora con una ESP32-CAM para reconocer figuras predefinidas y replicarlas. Como funcionalidad adicional, el robot puede jugar al ta-te-ti reflejando las jugadas en papel.

### Características principales
- Dibujo automático de figuras predefinidas (círculo, cuadrado, triángulo, cruz, estrella y corazón).
- Reconocimiento de imágenes mediante machine learning para replicar figuras dibujadas por el usuario.
- Juego de ta-te-ti, con jugadas registradas en una interfaz web y reflejadas en papel.
- Control basado en una EDU-CIAA para el manejo de motores y lógica del sistema.
- Interfaz web para seleccionar figuras y modos de uso.
- Comunicación WebSocket y UART entre los módulos.
  
### Tecnologías utilizadas
- Hardware: EDU-CIAA, ESP32-CAM, motores NEMA 17, controladores DRV8825, servo SG90.
- Software: C (para la EDU-CIAA), Arduino (para la ESP32-CAM), HTML/CSS/JavaScript para la interfaz web.
- Machine Learning: Edge Impulse para el reconocimiento de figuras.

Este proyecto fue desarrollado por el Grupo 2 en el marco de la materia Taller de Proyecto I en la Facultad de Ingeniería de la UNLP.
- Joaquín Chanquía
- Gabriel Ollier
- Melina Caciani Toniolo

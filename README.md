# Módulo Hardware - Práctica Sistemas de Tiempo Real

## Introducción

Este proyecto se corresponde con el desarrollo del módulo hardware en la práctica 1, apartado A, de la asignatura de "Sistemas de Tiempo Real" en el nuevo entorno establecido.

En esta práctica se propone desarrollar un sistema de control de tiempo real con la siguiente arquitectura:

* **Nodo Controlador**: Contiene toda la lógica de control del sistema. En la práctica se referencia a esta parte como Módulo Software.
* **Nodo Esclavo**: Simula el componente físico a controlar. En la práctica se referencia a esta parte como Módulo Hardware.
* **Entorno**: Se simula mediante un circuito electrónico

El escenario que se simula en la prática es el control remoto de una caretilla. Esta caretilla se corresponde con el "Nodo Esclavo" y debe captar información sobre el "Entorno" y posteriormente envíarla al "Nodo Controlador". Este procesará la información recibida y responderá a la carretilla con la acción a realizar.

El entorno de desarrollo usado hasta el momento es:

Arduino IDE -> Para desarrollar todo el código de del Módulo Hardware que ejecutará el un Arduino UNO
Wind River Workbanch -> Para desarrollar el código del Módulo Software. En este caso la ejecución se hace en una máquina virutal que se registra dentro del propio IDE. Esta máquina virual corre el sistema operativo de tiempo real VxWorks también desarrollado por Wind River.

El trabajo realizado con este proyecto adapta el código del Módulo Hardware a un entorno totalmente nuevo donde sustituimos el uso de Wind River Workbench por Eclipse y el uso de VxWorks por RTEMS. Dado que con estos cambios ya no se puede hacer uso de la máquina virtual de Wind River, los ejecutables pasaron a ejecutar a una placa física; RaspberryPi (RPI).

En este repositorio hablaremos de todo lo que concierne al desarrollo del Módulo Software en el nuevo entorno.

Para saber más sobre el módulo software: [Módulo Software - Práctica Sistemas de Tiempo Real](https://github.com/MarioPalomaresGallego/Practica1ASw)

## Objetivo

En lo que corresponde al Módulo Hardware el nuevo entorno realmente tiene muy pocas impliaciones. A nivel de implementación simplemente debemos cambiar la funciones de comunicación de USB a I2C. Por otro lado a nivel de desarrollo realmente se puede seguir usando el Arduino IDE para desarrollar la aplicación.

Dado que en el módulo software montamos un entorno completo sobre Eclipse, se decició integrar también sobre este el desarrollo de aplicacionese de Arduino.  De esta manera el alumno puede desarrollar ambos módulos de la práctica bajo el mismo IDE. A continuación mostramos como realizar esa integración.

**IMPORTANTE**: La explicación que se va a mostrar a continuación funciona para **Eclipse bajo el sitema operativo Ubuntu 18.04**. La **placa** Arduino usada se corresponde con un **Arduino UNO**.

## Eclipse

El desarrollo de Aplicaciones para Arduino viene integrado directamente dentro del paquete Eclipse CDT que se trata de un Plug-in para el desarrollo de aplicaciones C/C++, principalmente en sistemas empotrados. Para instalarlo puedes descargarte directamente una [versión de Elipse configurada](https://www.eclipse.org/downloads/packages/release/helios/sr2/eclipse-ide-cc-developers) por defecto con este  paquete o instalarlo manualmente. Para esto último deberás:

Help -> Eclipse MarketPlace -> Search "CDT" 

Deberás instalar el paquete llamado Eclipse C/C++ IDE CDT. Una vez termine la instalación te pedirá que reinicies Eclipse, cuando lo abras te aparecerá una nueva interfaz, la correspondiente a este paquete. Para crear el primer proyecto Arduino simplemente

File -> New Project -> C/C++ -> Arduino Proyect

La primera cosa extraña que vemos al crearse el poryecto es que el fichero contiene una extensión .cpp y no .ino, siendo esta úlitma la típica de los Sketch de Arduino. La única diferencia entre ambas es que en los .ino no tienes que incluuir cabeceras del sistema, de hecho si miramos el fichero .cpp creado, además de las típicas funciones setup() y loop() contiene la inclusión de la cabecera "Arduino.h".

Una vez desarrollamos el primer programa necesitamos especificar a Eclipse sobre que tipo de placa va a ejecutar. Esto lo realizamos desde el [Launch Bar](https://www.eclipse.org/community/eclipse_newsletter/2017/april/images/launch_bar_sm.png) de Eclipse. Esta barra nos muestra de izquierda a derecha la siguiente información:

* Tipo de ejecución: Actualmente solo de puede correr normal, no hay opción para depurar
* Proyecto: El poryecto a ejecutar
* Board: El tipo de placa sobre la que va a ejecutar

En el último apartado debemos cliquear "New Launch Target -> Arduino" y rellenar los campos de la siguiente forma

* Name: El que quieras
* Serial Port: El que haya asignado el kernel al conectar la placa. Se puede ver con el comando `dmesg | tail -20`
* Board Type: Arduino UNO
* Programer: El que viene por defecto

Con esto ya puedes lanzar los ejecutables sobre la placa una vez esta esté conectada. Para poder acceder a la consola del Arduino:

Shift+Ctrl+Alt+T -> Arduino Serial Monitor

Y rellenamos con los siguientes parámetros:

* Board: Nos debe aparecer el nombre que asinamos previamente al crear el "Launch Target".
* BaudRate: El mismo al que incie el programa el puerto serial

Con esto se nos debe crear una nueva consola donde veamos las impresiones del programa

## I2C

Como hemos comentado anteriormente, derivado del nuevo entorno, la comunicación que se realiza entre el "Módulo Software" y el "Módulo Hardware" pasa de ser USB a I2C. En Ardunio, para poder realizar un progrma con I2C deberas incluir la librería Wire.h

```C
#include "Wire.h"
```

Por defecto dentro de Eclipse esta librería se encuentra desactivada, para que Eclipse reconozca el fichero de cabecera anterior, deberás activarla manualmente:

Click Derecho en el Proyecto -> Properties -> Libraries 

Desplegamos la lista de librerías y seleccionamos Wire. De esta manera ya podrás en tus programas incluir el fichero de cabecera y las llamadas correspondientes sin recibir ningún error.

Como se puede ver además de la librería Wire tenemos otras librerías comunes. En caso de necesitar una libería adicional estas se pueden instalar de la siguiente forma:

Help -> Arduino Downloads Manager

Ahi podrás descargar tanto nuevas librerías como plataformas de desarrollo para cualquier tipo de placa Arduino.

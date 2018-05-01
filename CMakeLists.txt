cmake_minimum_required(VERSION 2.8)
project(Conculandia)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
add_executable(conculandia main.cpp Logger.cpp Personas/Turista.cpp Personas/Residente.cpp Lock/LockFile.cpp Seniales/SignalHandler.cpp Semaforo/Semaforo.cpp Generador.cpp Asistente.cpp Personas/Empleado.cpp Fifo/Fifo.cpp Fifo/FifoEscritura.cpp Fifo/FifoLectura.cpp Contadores.cpp)


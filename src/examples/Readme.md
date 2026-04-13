**Cómo compilar**

mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j4

# O solo los ejemplos:
make usart_example
make spi_example
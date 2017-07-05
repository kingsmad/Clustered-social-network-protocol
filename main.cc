/*************************************************************************
    > In god we trust
    > File Name: main.cpp
************************************************************************/
#include "src/socio_geo.h"

int main() {
  freopen("../input.txt", "r", stdin);
  socio::Graph graph;
  graph.Init();
  graph.Run(1000);
}

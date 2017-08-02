/*************************************************************************
    > In god we trust
    > File Name: main.cpp
************************************************************************/
#include <stdio.h>
#include <string.h>
#include "src/socio_geo.h"

bool debug = false;

int main(int argc, char** argv) {
  freopen("../input.txt", "r", stdin);
  socio::Graph graph;
  graph.Init();

  int rounds = 100000;

  if (argv[1]) {
    if (sscanf(argv[1], "--rounds=%d", &rounds) == -1) {
      printf("please enter rounds! : run --rounds=1000\n");
      return 0;
    }
  }

  if (argv[2]) {
    char s[100];
    sscanf(argv[2], "--debug=%s", s);

    if (strcmp(s, "true") == 0) { debug = true; }
  }

  printf("Running for %d rounds, debug mode is %s\n", rounds,
         debug ? "on" : "off");

  // printf("Running for total %d rounds\n", rounds);
  graph.Run(rounds);

  return 0;
}

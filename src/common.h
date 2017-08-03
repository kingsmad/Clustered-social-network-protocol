const int kRowSize = 100;
const int kColSize = kRowSize;



const int kAlpha = 3;
const int kSnapshotPerRound = 1800;
const int kThreadNum = 16;
const int kParalleSize = kRowSize * kColSize / (kThreadNum*8) + 10;

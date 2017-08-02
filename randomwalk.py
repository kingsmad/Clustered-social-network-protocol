import numpy as np
import random
import time
import sys

kMaxRow = 500
kMaxMsgCnt = 100
kMaxPosCnt = 100
kNodeNum = 500
kMsgNum = 1000

def gen_random_pos(tot_pos):
    print("%d " %(tot_pos))
    while tot_pos > 0:
        x = random.randint(0, kMaxRow-1)
        y = random.randint(0, kMaxRow-1)
        print ("%d %d " %(x, y))
        tot_pos -= 1
    print("\n")

def gen_random_nodes(nodes_num):
    print("%d\n" %(nodes_num))
    while nodes_num > 0:
        pos_cnt = random.randint(2, kMaxPosCnt - 1)
        gen_random_pos(pos_cnt)
        nodes_num -= 1

def gen_random_msg(msg_tot_cnt, max_node_idx):
    print(msg_tot_cnt)
    while msg_tot_cnt > 0:
        src = random.randint(0, max_node_idx-1)
        dst = random.randint(0, max_node_idx-1)
        while dst == src:
            dst = random.randint(0, max_node_idx-1)
        cnt = random.randint(1, kMaxMsgCnt-1)
        print ("%d %d %d\n" %(src, dst, cnt))
        msg_tot_cnt -= 1

def main():
    sys.stdout = open("nodes.txt", "w")

    # generate nodes with random positions
    gen_random_nodes(kNodeNum)

    # generate random messages
    gen_random_msg(kMsgNum, kNodeNum)

if __name__ == '__main__':
    main()

import random

def create_msg(f1, f2, msg_num):
    total_nodes = 0
    with open(f1, "r") as f:
        for i, each in enumerate(f):
            if i > 0:
                break
            total_nodes += int(each)

    #current simulation is one node can send msg to multiple receivers vice versa
    msg_node = random.randrange(1, total_nodes * total_nodes / 4 - 1)

    k = 0
    src_dest_pair = []
    check = set()
    while k < msg_node:
        src = random.randrange(0, total_nodes - 1)
        dest = random.randrange(0, total_nodes - 1)
        mn = random.randrange(0, len(msg_num))
        if src != dest:
            if (src, dest) not in check:
                k += 1
                src_dest_pair.append((src, dest, msg_num[mn]))
            check.add((src, dest))

    with open(f2, "w") as f:
        print(msg_node, file=f, end='\n')
        for each_pair in src_dest_pair:
            res = list(map(str, each_pair))
            print(" ".join(res), file=f, end='\n')


def merge(f1, f2, f3):
    with open(f3, "w") as f:
        with open(f1, "r") as fr1:
            for each in fr1:
                print(each[:-1], file=f, end='\n')
        print("", file=f, end='\n')
        with open(f2, "r") as fr2:
            for each in fr2:
                print(each[:-1], file=f, end='\n')


def main():
    node_file = "nodes.txt"
    msg_file = "msg.txt"
    input_file = "input1.txt"
    #possible msg numbers send from one node
    msg_num = [10, 50, 100, 200]
    create_msg(node_file, msg_file, msg_num)

    merge(node_file, msg_file, input_file)


if __name__ == '__main__':
    main()

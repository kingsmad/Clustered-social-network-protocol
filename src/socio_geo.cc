#include "socio_geo.h"
#include <stdarg.h>
#include <algorithm>
#include <memory>
#include <set>
#include "common.h"

extern bool debug;

static void buginfo(const char* f, ...) {
  if (!debug) return;
  va_list al;
  va_start(al, f);
  vprintf(f, al);
  va_end(al);
}

namespace socio {

Node::Node(int ssid, int tot_node_num, int places_cnt)
    : sid_(ssid), stat_(0), generator_(rd_()), uni_(0, places_cnt - 1) {
  Init(tot_node_num, places_cnt);
}

Node::~Node() {
  for (auto it = seq2msg_.begin(); it != seq2msg_.end(); ++it) {
    delete it->second;
  }
}

void Node::Init(int tot_node_num, int places_cnt) {
  enc_pr_.resize(tot_node_num);
  enc_count_.resize(tot_node_num);
  for (size_t i = 0; i < places_cnt; ++i) {
    int x, y;
    scanf("%d%d", &x, &y);
    places_.emplace_back(x, y);
  }
  xp_ = places_.at(0).first;
  yp_ = places_.at(0).second;
  current_dst_idx_ = 1;
}

void Node::AddMsg(int seq, int num, int src, int dst) {
  buginfo("Msg is added! : (%d, %d, %d, %d)\n", seq, src, dst, num);
  if (dst == sid()) {
    stat() += num;
    return;
  }
  auto it = seq2msg_.find(seq);
  if (it != seq2msg_.end()) {
    it->second->cnt += num;
  } else {
    Message* msg = new Message(seq, src, dst, num);
    seq2msg_.emplace(seq, msg);
  }
  buf_sz() += num;
}

void Node::RemoveMsg(int seq, int num) {
  auto it = seq2msg_.find(seq);
  if (it == seq2msg_.end()) { return; }
  Message* msg = it->second;
  msg->cnt -= num;
  buf_sz() += num;
  if (msg->cnt == 0) {
    delete msg;
    seq2msg_.erase(it);
  }
}

// position will change
std::pair<int, int> Node::NextPosition() {
  if (places().size() == 1) { return places().front(); }
  std::pair<int, int>& cp = places().at(current_dst_idx_);
  if (cp.first > xp()) {
    ++xp();
  } else if (cp.first < xp()) {
    --xp();
  } else if (cp.second > yp()) {
    ++yp();
  } else if (cp.second < yp()) {
    --yp();
  }

  if (xp() == cp.first && yp() == cp.second) {
    current_dst_idx_ = uni_(generator_);
    // buginfo("Current idx of node %d is : %d\n", sid(), current_dst_idx_);
  }
  // buginfo("The next position of node %d is: %d %d\n", sid(), xp(), yp());
  return std::make_pair(xp(), yp());
}

bool Node::HasMsg(int seq) { return seq2msg_.count(seq) > 0; }

void Node::Encounter(Node* enc_node) {
  buginfo("Node %d encounters node %d\n", sid(), enc_node->sid());
  std::vector<std::pair<int, Message*>> pairs;
  for (auto pair : seq2msg_) { pairs.emplace_back(pair.first, pair.second); }

  for (auto pair : pairs) {
    Message* msg = pair.second;
    int seq = pair.first;
    if (msg->dst == enc_node->sid()) {
      enc_node->AddMsg(seq, msg->cnt, msg->src, msg->dst);
      buginfo("Meet destination! Transfer %d msgs from %d to %d\n", msg->cnt,
              msg->src, msg->dst);
      RemoveMsg(seq, msg->cnt);
    } else if (enc_node->HasMsg(seq)) {
      continue;
    } else {
      if (msg->cnt > 1) {
        int forward_cnt = msg->cnt - msg->cnt / 2;
        buginfo("Forwarding %d msgs from %d to %d\n", forward_cnt, msg->src,
                msg->dst);
        enc_node->AddMsg(seq, forward_cnt, msg->src, msg->dst);
        RemoveMsg(seq, forward_cnt);
      } else if (msg->cnt == 1) {
        if (enc_node->EnPr(msg->dst) > this->EnPr(msg->dst)
            || enc_node->EnCount(msg->dst) > kAlpha) {
          enc_node->AddMsg(seq, 1, msg->src, msg->dst);
          buginfo("Forwarding 1 msgs from %d to %d\n", msg->src, msg->dst);
          RemoveMsg(seq, 1);
        }
      }
    }
  }
}

Message::Message(int sseq, int ssrc, int ddst, int ccnt)
    : seq(sseq), src(ssrc), dst(ddst), cnt(ccnt) {}

void Graph::Run(int cnt) {
  for (int i = 0; i < cnt; ++i) {
    for (Node* p : nodes_) { move(p); }
    int64_t st = 0;
    int64_t pos_sz = kRowSize * kColSize;
    while (st < pos_sz) {
      int64_t ed = std::min(st + kParalleSize, pos_sz);
      cpu_stream_.SendWork([st, ed, this]() {
        for (int64_t i = st; i < ed; ++i) {
          auto it = pos2nodes_.find(i);
          if (it == pos2nodes_.end()) continue;
          std::unordered_set<Node*>& nset = it->second;
          for (Node* p : nset) {
            for (Node* q : nset) {
              if (p != q) {
                p->Encounter(q);
                ++this->nodes_[q->sid()]->EnCount(p->sid());
                ++this->nodes_[p->sid()]->EnCount(q->sid());
              }
            }
          }
        }
      });
      st = ed;
    }
    cpu_stream_.SyncStream();
    if (i % kSnapshotPerRound == 0) {
      printf("Snapshot in round: %d\n", i / 1800);
      Statistic();
    }
  }

  for (Node* o : nodes_) { delete o; }
}

void Graph::Statistic() {
  printf("Msgs received by every nodes:\n");
  for (Node* o : nodes_) { printf("%d ", o->stat()); }
  printf("\n");

  int64_t msg_cnt = 0;
  for (Node* o : nodes_) msg_cnt += o->stat();
  int64_t msg_tot = 0;
  for (auto pair : nodes_exp_msg_cnt_) msg_tot += pair.second;
  printf("total recv-percentage is : %f\n",
         static_cast<double>(msg_cnt) / msg_tot);

  for (auto pair : nodes_exp_msg_cnt_) {
    int sid = pair.first;
    int exp_cnt = pair.second;
    printf(
        "Node %d expected %d messages, received %d msgs, success rate is %f\n ",
        sid, exp_cnt, nodes_[sid]->stat(),
        static_cast<double>(nodes_[sid]->stat()) / exp_cnt);
  }
  printf("\n");
}

inline int64_t Graph::TwoDim2One(const std::pair<int, int>& pair) {
  return pair.first * kRowSize + pair.second;
}

void Graph::move(Node* o) {
  int64_t cur_pos = TwoDim2One(std::make_pair(o->xp(), o->yp()));
  int64_t nxt_pos = TwoDim2One(o->NextPosition());
  pos2nodes_[cur_pos].erase(o);
  pos2nodes_[nxt_pos].insert(o);
}

void Graph::Init() {
  // init nodes
  int nodes_cnt;
  scanf("%d", &nodes_cnt);
  for (size_t i = 0; i < nodes_cnt; ++i) {
    int places_cnt;
    scanf("%d", &places_cnt);
    Node* p = new Node(i, nodes_cnt, places_cnt);
    nodes_.emplace_back(p);
  }

  // init messages
  int msg_cnt;
  scanf("%d", &msg_cnt);
  for (int i = 0; i < msg_cnt; ++i) {
    int src, dst, num;
    scanf("%d%d%d", &src, &dst, &num);
    nodes_[src]->AddMsg(i, num, src, dst);
    nodes_exp_msg_cnt_[dst] += num;
  }

  // caculate pr
  std::set<std::pair<int, int>> pset;
  for (size_t i = 0; i < nodes_cnt; ++i) {
    pset.clear();
    for (std::pair<int, int>& p : nodes_.at(i)->places()) { pset.emplace(p); }
    int i_sz = nodes_.at(i)->places().size();
    for (size_t j = i + 1; j < nodes_cnt; ++j) {
      int cnt = 0;
      for (std::pair<int, int>& q : nodes_.at(j)->places()) {
        if (pset.count(q)) { ++cnt; }
      }
      nodes_[i]->EnPr(j) =
          static_cast<double>(cnt) / nodes_.at(j)->places().size() * i_sz;
      nodes_[j]->EnPr(j) = nodes_[i]->EnPr(j);
    }
  }
  for (size_t i = 0; i < nodes_.size(); ++i) {
    for (size_t j = 0; j < nodes_.size(); ++j) { nodes_[i]->EnCount(j) = 0; }
  }
}

}  // namespace socio

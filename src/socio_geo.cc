#include "socio_geo.h"
#include <algorithm>
#include <set>

namespace socio {

Node::Node(int ssid) : sid_(ssid) { Init(); }

void Node::Init() {
  int places_cnt;
  scanf("%d", &places_cnt);
  for (size_t i = 0; i < places_cnt; ++i) {
    int x, y;
    scanf("%d%d", &x, &y);
    places_.emplace_back(x, y);
  }
  xp_ = places_.at(0).first;
  yp_ = places_.at(0).second;
  current_dst_idx_ = 1;
}

void Node::AddNewMsg(int seq, int src, int dst, int num) {
  Message* msg = new Message(seq, src, dst, num);
  seq2msg_.emplace(seq, msg);
  BufferSize() += num;
}

void Node::DelMsg(int seq) {
  if (seq2msg_.count(seq) == 0) { return; }
  Message* msg = seq2msg_.at(seq);
  BufferSize() += msg->copy_cnt;
  delete (msg);
}

void Node::DelMsg(Message* msg) {
  BufferSize() += msg->copy_cnt;
  delete (msg);
}

void Node::Forward(Message* msg, Node* dst) {
  if (msg->dst == dst->Sid()) {
    dst->Stat() += msg->copy_cnt;
    DelMsg(msg);
    return;
  }

  int copy_cnt = msg->copy_cnt;
  msg->copy_cnt = copy_cnt / 2;

  Message* new_msg =
      new Message(msg->seq, msg->src, msg->dst, copy_cnt - msg->copy_cnt);
  dst->seq2msg_.emplace(new_msg->seq, new_msg);
  this->BufferSize() -= new_msg->copy_cnt;
  dst->BufferSize() += new_msg->copy_cnt;
}

// position will change
std::pair<int, int> Node::NextPosition() {
  if (Places().size() == 1) { return Places().front(); }
  int dx = 0, dy = 0;
  std::pair<int, int>& cp = Places().at(current_dst_idx_);
  if (cp.first != xp()) {
    dx = (cp.first > xp()) ? 1 : -1;
  } else if (cp.second != yp()) {
    dy = (cp.first > yp()) ? 1 : -1;
  }
  xp() = xp() + dx * speed_;
  yp() = yp() + dy * speed_;

  if (xp() == cp.first && yp() == cp.second) {
    current_dst_idx_ = (current_dst_idx_ + 1) % Places().size();
  }

  return std::make_pair(xp(), yp());
}

bool Node::HasMsg(int seq) { return seq2msg_.count(seq) > 0; }

void Node::Encounter(Node* enc_node) {
  std::vector<int> erase_seq_buffer;
  for (auto pair : seq2msg_) {
    int seq = pair.first;
    Message* msg = pair.second;
    if (msg->dst == enc_node->Sid()) {
      Forward(msg, enc_node);
    } else if (enc_node->HasMsg(seq)) {
      continue;
    } else {
      if (msg->copy_cnt > 1) {
        Forward(msg, enc_node);
      } else if (msg->copy_cnt == 1) {
        if (enc_node->EnPr(msg->dst) > this->EnPr(msg->dst)
            || enc_node->EnCount(msg->dst) > alpha) {
          Forward(msg, enc_node);
          erase_seq_buffer.push_back(msg->seq);
        }
      }
    }
  }
  for (int seq : erase_seq_buffer) {
    auto it = seq2msg_.find(seq);
    delete it->second;
    seq2msg_.erase(it);
  }
}

Message::Message(int sseq, int ssrc, int ddst, int ccnt)
    : seq(sseq), src(ssrc), dst(ddst), copy_cnt(ccnt) {}

void Graph::Run(int cnt) {
  while (cnt--) {
    for (Node* p : nodes_) { move(p); }
    for (auto it : pos2nodes_) {
      std::unordered_set<Node*>& nset = it.second;
      for (Node* p : nset)
        for (Node* q : nset) {
          if (p != q) { p->Encounter(q); }
        }
    }
  }
}

inline int64_t Graph::TwoDim2One(const std::pair<int, int>& pair) {
  return pair.first * ROW_SZ + pair.second;
}

void Graph::move(Node* o) {
  int64_t cur_pos = TwoDim2One(std::make_pair(o->xp(), o->yp()));
  int64_t nxt_pos = TwoDim2One(o->NextPosition());
  pos2nodes_[cur_pos].erase(o);
  pos2nodes_[nxt_pos].insert(o);
}

void Graph::Init() {
  // init nodes
  int nodes_cnt = 0;
  scanf("%d", &nodes_cnt);
  for (size_t i = 0; i < nodes_cnt; ++i) {
    Node* p = new Node(i);
    nodes_.emplace_back(p);
  }

  // init messages
  int msg_cnt;
  scanf("%d", &msg_cnt);
  for (int i = 0; i < msg_cnt; ++i) {
    int src, dst, num;
    scanf("%d%d%d", &src, &dst, &num);
    nodes_[src]->AddNewMsg(i, src, dst, num);
  }

  // caculate pr
  std::set<std::pair<int, int>> pset;
  for (size_t i = 0; i < nodes_cnt; ++i) {
    pset.clear();
    for (std::pair<int, int>& p : nodes_.at(i)->Places()) { pset.emplace(p); }
    int i_sz = nodes_.at(i)->Places().size();
    for (size_t j = i + 1; j < nodes_cnt; ++j) {
      int cnt = 0;
      for (std::pair<int, int>& q : nodes_.at(j)->Places()) {
        if (pset.count(q)) { ++cnt; }
      }
      nodes_[i]->EnPr(j) =
          static_cast<double>(cnt) / nodes_.at(j)->Places().size() * i_sz;
    }
  }
  for (size_t i = 0; i < MAXNODE; ++i) {
    for (size_t j = 0; j < MAXNODE; ++j) { nodes_[i]->EnCount(j) = 0; }
  }
}

}  // namespace socio

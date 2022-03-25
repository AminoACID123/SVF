#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <llvm/IR/Dominators.h>
#include <llvm/Analysis/PostDominators.h>
#include <llvm/ADT/SmallVector.h>
#include <fstream>
#include <queue>

using namespace llvm;
using namespace std;

using B2Imap = map<BasicBlock*, int>;
using I2Bmap = map<int, BasicBlock*>;
using DTNode = llvm::DomTreeNodeBase<BasicBlock>;

extern unsigned _TheBB;
extern unsigned _IDom;
extern unsigned _Level;
extern unsigned _Children;
extern unsigned _DFSNumIn;
extern unsigned _DFSNumOut;


// Dummy class to get the offset of private members.
template <class NodeT> class MyDomTreeNodeBase {
  
    NodeT *TheBB;
    MyDomTreeNodeBase *IDom;
    unsigned Level;
    SmallVector<MyDomTreeNodeBase *, 4> Children;
    mutable unsigned DFSNumIn = ~0;
    mutable unsigned DFSNumOut = ~0;

public:
    MyDomTreeNodeBase(NodeT *BB, MyDomTreeNodeBase *iDom)
      : TheBB(BB), IDom(iDom), Level(IDom ? IDom->Level + 1 : 0) {}

    void init()
    {
        _TheBB = getTheBBOff();
        _IDom = getIDomOff();
        _Level = getLevelOff();
        _Children = getChildrenOff();
        _DFSNumIn = getDFSNumInOff();
        _DFSNumOut = getDFSNumOutOff();

    }

private:
    unsigned getTheBBOff(){ return (char*)(&TheBB) - (char*)this; }
    unsigned getIDomOff(){ return (char*)(&IDom) - (char*)this; }
    unsigned getLevelOff(){ return (char*)(&Level) - (char*)this; }
    unsigned getChildrenOff(){ return (char*)(&Children) - (char*)this; }
    unsigned getDFSNumInOff(){ return (char*)(&DFSNumIn) - (char*)this; }
    unsigned getDFSNumOutOff(){ return (char*)(&DFSNumOut) - (char*)this; }
};


class _TreeNode
{
public:
    int TheBB; // BasicBlock
    int IDom;   // Immediate Dominator
    vector<int> Children; // Children Nodes
    unsigned int level;   // Level in Tree
    unsigned int DFSNumIn;  // DFS Order (In)
    unsigned int DFSNumOut; // DFS Order (Out)
    _TreeNode(){}
    _TreeNode(DTNode& node);
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {  
        ar & TheBB;
        ar & IDom;
        ar & Children;
        ar & level;
        ar & DFSNumIn;
        ar & DFSNumOut;
    }   
};


class _DominatorTree
{
public:
    string m_name;  // Module name
    string f_name;  // Function name
    vector<_TreeNode> nodes;    // Tree nodes
    int root;   // Root ID
    _DominatorTree() {}

    template <class Archive>
    void serialize(Archive &ar,const unsigned int version)
    {  
        ar & m_name;
        ar & f_name;
        ar & nodes;
        ar & root;
    }  
};

extern map<string, _DominatorTree*> DT;
extern map<string, _DominatorTree*> PDT;


class DominatorTreeWrapper: public DominatorTree
{
public:

    DominatorTreeWrapper(Function& F)
    {
        string m_name = F.getParent()->getName().str();
        string f_name = F.getName().str();
        string key = m_name + ":" + f_name;
        auto iter = DT.find(key);
        if(iter == DT.end())
        {
            llvm::outs() << "Calculating from scratch\n";
            recalculate(F);
            updateDFSNumbers();
            _save();
        }
        else
        {
            llvm::outs() << "Loading from existing\n";
            _load(iter->second);
        }
    }

    friend class _TreeNode;

private:
    void _save();
    void _load(_DominatorTree* dt);

    DTNode* createNode(I2Bmap& i_block ,_TreeNode& node);
    B2Imap block_i;
    I2Bmap i_block;

};

/*
class PostDominatorTreeWrapper: public PostDominatorTree{
public:

    PostDominatorTreeWrapper(Function& F)
    {
        string m_name = F.getParent()->getName().str();
        string f_name = F.getName().str();
        string key = m_name + ":" + f_name;
        auto iter = PDT.find(key);
        if(iter == PDT.end())
        {
            recalculate(F);
            _save();
        }
        else
        {
            _load(iter->second);
        }
    }
    void setModules(map<string, Module*>* modules);

    friend class _TreeNode;
private:
    void _save();
    void _load(_DominatorTree* dt);
    map<string, Module*>* modules;
    DTNode* createNode(I2Bmap& i_block ,_TreeNode& node);
    B2Imap block_i;
    I2Bmap i_block;

};

*/
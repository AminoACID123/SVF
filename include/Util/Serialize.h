#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
//#include <boost/serialization/vector.hpp>
//#include <boost/serialization/map.hpp>
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
    int TheBB;
    int IDom;
    vector<int> Children;
    unsigned int level;
    unsigned int DFSNumIn;
    unsigned int DFSNumOut;
    _TreeNode(){}
    _TreeNode(DTNode& node);
};

class _DominatorTree
{
public:
    string m_name; 
    string f_name;
    vector<_TreeNode> nodes;
    int root;
    _DominatorTree() {}
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
            recalculate(F);
            updateDFSNumbers();
            _save();
        }
        else
        {
            _load(iter->second);
        }
    }

    void setModules(map<string, Module*>* modules);

    friend class _TreeNode;

    friend class boost::serialization::access;
 
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{	

		ar  & "hello"; 
    }


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
#include "Util/Serialize.h"

// Offset of DomTreeNodeBase members
unsigned _TheBB;
unsigned _IDom;
unsigned _Level;
unsigned _Children;
unsigned _DFSNumIn;
unsigned _DFSNumOut;


map<string, _DominatorTree*> DT;
map<string, _DominatorTree*> PDT;
extern map<string, Module*> modules;

template <typename K, typename V>
inline static V getValue(map<K, V>& _map, K _key)
{
    auto iter = _map.find(_key);
    if(iter == _map.end())
    {
        llvm::outs()<<"Unrecognized Key Value!\n";
        exit(-1);
    }
    return iter->second;
}

_TreeNode::_TreeNode(DTNode& node)
{
    level = node.getLevel();
    DFSNumIn = node.getDFSNumIn();
    DFSNumOut = node.getDFSNumOut();
}



void DominatorTreeWrapper::_save() 
{
    _DominatorTree* tree = new _DominatorTree();
    tree->m_name = Parent->getParent()->getName().str();        
    tree->f_name = Parent->getName().str();

    block_i[NULL] = 0;
    int i = 1;
    for(auto iter = Parent->getBasicBlockList().begin(),
        e=Parent->getBasicBlockList().end();iter!=e;iter++)
    {
        block_i[&(*iter)] = i;
        i++;
    }

    // Save all nodes
    for(auto iter = DomTreeNodes.begin(),e=DomTreeNodes.end();iter!=e;iter++)
    {
        tree->nodes.push_back(*(iter->second));
        tree->nodes.back().TheBB = getValue(block_i, iter->first);
        auto IDom = iter->second->getIDom();
        if(IDom)
        {
            tree->nodes.back().IDom = getValue(block_i, IDom->getBlock());
        }
        else
        {
            // This is root
            tree->nodes.back().IDom = -1;
            tree->root = tree->nodes.back().TheBB;
        }
        for(auto child=iter->second->begin(),end=iter->second->end();child!=end;child++)
        {
            int block = getValue(block_i, (*child)->getBlock());
            tree->nodes.back().Children.push_back(block);
        }
    }


    DT[tree->m_name + ":" + tree->f_name] = tree;
}



DTNode* DominatorTreeWrapper:: 
createNode(I2Bmap& i_block ,_TreeNode& node)
{
    DTNode* n;
    if(node.TheBB == -1)
    {
        n = new DTNode(NULL, NULL);
    }
    else
    {
        n = new DTNode(getValue(i_block, node.TheBB), NULL);
    }
    
    *(unsigned*)((char*)n + _Level) = node.level;
    *(unsigned*)((char*)n + _DFSNumIn) = node.DFSNumIn;
    *(unsigned*)((char*)n + _DFSNumOut) = node.DFSNumOut;
    return n;
}


void DominatorTreeWrapper:: _load(_DominatorTree* dt)
{
    auto mod = getValue(modules, dt->m_name);
    auto f = mod->getFunction(dt->f_name);
    if(!f)
    {
        exit(-1);
    }

    i_block[0] = NULL;
    int i = 1;
    for(auto iter = f->getBasicBlockList().begin(),
        e=f->getBasicBlockList().end();iter != e;iter++)
    {
        i_block[i] = &(*iter);
        i++;
    }

    Parent = f;
    DFSInfoValid = true;
    SlowQueries = 0;


    // Load DomTreeNodeMap
    map<BasicBlock*, DTNode*> temp;
    for(auto iter = dt->nodes.begin(),
        e=dt->nodes.end();iter!=e;iter++)
    {
        BasicBlock* block = getValue(i_block, iter->TheBB);
        DTNode* n = new DTNode(block, NULL);
        temp[block] = n;
    
        *(unsigned*)((char*)n + _Level) = iter->level;
        *(unsigned*)((char*)n + _DFSNumIn) = iter->DFSNumIn;
        *(unsigned*)((char*)n + _DFSNumOut) = iter->DFSNumOut;
        //DomTreeNodes.insert(make_pair(block, unique_ptr<DTNode>(n)));

        if(iter->IDom == -1)
        {
            // Load RootNode and Roots
            RootNode = n;
            Roots.push_back(block);
        }

    }

    for(auto iter = dt->nodes.begin(),
        e=dt->nodes.end();iter!=e;iter++)
    {
        BasicBlock* block = getValue(i_block, iter->TheBB);
        DTNode* n = temp[block];
        for(auto c=iter->Children.begin(),ce=iter->Children.end();c!=ce;c++)
        {
            DTNode* child = temp[getValue(i_block, *c)];
            ((SmallVector<DTNode*, 4>*)((char*)n + _Children))->push_back(child);
        }
        if(iter->IDom != -1)
        {
            DTNode* idom = temp[getValue(i_block, iter->IDom)];
            *(DTNode**)((char*)n + _IDom) = idom;
        }
    }

    for(auto iter = temp.begin(),e=temp.end();iter!=e;iter++)
    {
        DomTreeNodes.insert(make_pair(iter->first, unique_ptr<DTNode>(iter->second)));
    }

}
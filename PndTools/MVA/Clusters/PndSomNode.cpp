/* ***********************************************
 * Self Organizing Map node class implementation *
 * Author: M.Babai@rug.nl                        *
 * Version:                                      *
 * LICENSE:                                      *
 * ***********************************************
 */

#include <iostream>

#include "PndSomNode.h"

/**
 * Default constructor.
 */
PndSomNode::PndSomNode()
  : m_iLeft(-1),
    m_iTop(-1),
    m_iRight(-1),
    m_iBottom(-1),
    m_xPos(0.0),
    m_yPos(0.0),
    m_weightDim(0),
    m_label("NO_LABEL"),
    m_Weights(),
    m_RespNodeIndex(),
    m_NeighborList()
{}
/**
 * Constructor
 *@param WeightsDim Dimension of the weights.
 */
PndSomNode::PndSomNode(size_t WeightsDim)
  : m_iLeft(-1),
    m_iTop(-1),
    m_iRight(-1),
    m_iBottom(-1),
    m_xPos(0.0),
    m_yPos(0.0),
    m_weightDim(WeightsDim),
    m_label("NO_LABEL"),
    m_Weights(),
    m_RespNodeIndex(),
    m_NeighborList()
{}

/**
 * Constructor
 *@param weight Weights of the current node.
 */
PndSomNode::PndSomNode(std::vector<float> const& weight)
  : m_iLeft(-1),
    m_iTop(-1),
    m_iRight(-1),
    m_iBottom(-1),
    m_xPos(0.0),
    m_yPos(0.0),
    m_weightDim( weight.size() ),
    m_label("NO_LABEL"),
    m_Weights(weight),
    m_RespNodeIndex(),
    m_NeighborList()
{}

/**
 * Constructor
 *@param label Assigned label to the current node.
 *@param weight Weights of the current node.
 */
PndSomNode::PndSomNode( std::string const& label, std::vector<float> const& weight)
  : m_iLeft(-1),
    m_iTop(-1),
    m_iRight(-1),
    m_iBottom(-1),
    m_xPos(0.0),
    m_yPos(0.0),
    m_weightDim( weight.size() ),
    m_label(label),
    m_Weights(weight),
    m_RespNodeIndex(),
    m_NeighborList()
{}

/**
 * Constructor
 *@param lft Left neighbor.
 *@param top Top neighbor.
 *@param rgt Right neighbor.
 *@param bot Bottom neighbor.
 *@param WeightsDim Dimension of the weights.
 */
PndSomNode::PndSomNode(int lft, int top, int rgt, int bot, size_t WeightsDim)
  : m_iLeft(lft),
    m_iTop(top),
    m_iRight(rgt),
    m_iBottom(bot),
    m_xPos(0.0),
    m_yPos(0.0),
    m_weightDim(WeightsDim),
    m_label("NO_LABEL"),
    m_Weights(),
    m_RespNodeIndex(),
    m_NeighborList()
{}

/**
 * Constructor
 *@param lft Left neighbor.
 *@param top Top neighbor.
 *@param rgt Right neighbor.
 *@param bot Bottom neighbor.
 *@param WeightsDim Dimension of the weights.
 *@param label Assigned label to the current node.
 */
PndSomNode::PndSomNode(int lft, int top, int rgt, int bot, size_t WeightsDim,
                       std::string const& label)
  : m_iLeft(lft),
    m_iTop(top),
    m_iRight(rgt),
    m_iBottom(bot),
    m_xPos(0.0),
    m_yPos(0.0),
    m_weightDim(WeightsDim),
    m_label(label),
    m_Weights(),
    m_RespNodeIndex(),
    m_NeighborList()
{}

/**
 * Constructor
 *@param lft Left neighbor.
 *@param top Top neighbor.
 *@param rgt Right neighbor.
 *@param bot Bottom neighbor.
 *@param WeightsDim Dimension of the weights.
 *@param label Assigned label to the current node.
 *@param weight Weights of the current node.
 */
PndSomNode::PndSomNode(int lft, int top, int rgt, int bot,
                       std::string const& label, std::vector<float> const& weight)
  : m_iLeft(lft),
    m_iTop(top),
    m_iRight(rgt),
    m_iBottom(bot),
    m_xPos(0.0),
    m_yPos(0.0),
    m_weightDim(weight.size()),
    m_label(label),
    m_Weights(weight),
    m_RespNodeIndex(),
    m_NeighborList()
{}

// Copy Constructor
PndSomNode::PndSomNode(PndSomNode const &oth)
 : m_iLeft(oth.m_iLeft),
   m_iTop(oth.m_iTop),
   m_iRight(oth.m_iRight),
   m_iBottom(oth.m_iBottom),
   m_xPos(oth.m_xPos),
   m_yPos(oth.m_yPos),
   m_weightDim(oth.m_weightDim),
   m_label(oth.m_label),
   m_Weights(oth.m_Weights),
   m_RespNodeIndex(oth.m_RespNodeIndex),
   m_NeighborList(oth.m_NeighborList)
{}

// Operators
PndSomNode& PndSomNode::operator=(PndSomNode const &oth)
{
  this->m_iLeft         = oth.m_iLeft;
  this->m_iTop          = oth.m_iTop;
  this->m_iRight        = oth.m_iRight;
  this->m_iBottom       = oth.m_iBottom;
  this->m_xPos = oth.m_xPos;
  this->m_yPos = oth.m_yPos;
  this->m_weightDim     = oth.m_weightDim;
  this->m_label         = oth.m_label;
  this->m_Weights       = oth.m_Weights;
  this->m_RespNodeIndex = oth.m_RespNodeIndex;
  this->m_NeighborList  = oth.m_NeighborList;

  return (*this);
}

PndSomNode::~PndSomNode()
{
  ClearInternalStructures();
}

void PndSomNode::ClearInternalStructures()
{
  m_Weights.clear();
  m_RespNodeIndex.clear();
  m_NeighborList.clear();
}

// Adjust the weights for the current node.
void AdjustWeights( std::vector<double> const& target,
                    double LearningRate, double Influence)
{
  target.size();
  std::cout << LearningRate << " " << Influence
            <<'\n';
}

///////__________ DEBUG FUNCTIONS _______________
#if (PRINT_SOMNODE_DEBUG > 0)
void PndSomNode::PrintNode() const
{
  std::cerr << "<DEBUG_INFO> Printing the info on the current node\n"
            << "\t m_weightDim = " << m_weightDim
            << " m_label = " << m_label
            << " length (m_Weights) = " << m_Weights.size()
            <<"\n";
}
#endif
///////__________ END DEBUG FUNCTIONS ____________

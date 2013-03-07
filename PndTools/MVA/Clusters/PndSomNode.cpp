/* ***********************************************
 * Self Organizing Map node class implementation *
 * Author: M.Babai@rug.nl                        *
 * Version:                                      *
 * LICENSE:                                      *
 * ***********************************************
 */

//________________________
#include <iostream>

//________________________
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
    m_NeighbourList(),
    m_labelCnts()
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
    m_NeighbourList(),
    m_labelCnts()
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
    m_NeighbourList(),
    m_labelCnts()
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
    m_NeighbourList(),
    m_labelCnts()
{}

/**
 * Constructor
 *@param lft Left neighbour.
 *@param top Top neighbour.
 *@param rgt Right neighbour.
 *@param bot Bottom neighbour.
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
    m_NeighbourList(),
    m_labelCnts()
{}

/**
 * Constructor
 *@param lft Left neighbour.
 *@param top Top neighbour.
 *@param rgt Right neighbour.
 *@param bot Bottom neighbour.
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
    m_NeighbourList(),
    m_labelCnts()
{}

/**
 * Constructor
 *@param lft Left neighbour.
 *@param top Top neighbour.
 *@param rgt Right neighbour.
 *@param bot Bottom neighbour.
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
    m_NeighbourList(),
    m_labelCnts()
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
   m_NeighbourList(oth.m_NeighbourList),
   m_labelCnts(oth.m_labelCnts)
{}

// Operators
PndSomNode& PndSomNode::operator=(PndSomNode const &oth)
{
  // Check for self-assignment
  if (this != &oth)
  {
    this->m_iLeft         = oth.m_iLeft;
    this->m_iTop          = oth.m_iTop;
    this->m_iRight        = oth.m_iRight;
    this->m_iBottom       = oth.m_iBottom;
    this->m_xPos          = oth.m_xPos;
    this->m_yPos          = oth.m_yPos;
    this->m_weightDim     = oth.m_weightDim;
    this->m_label         = oth.m_label;
    this->m_Weights       = oth.m_Weights;
    this->m_RespNodeIndex = oth.m_RespNodeIndex;
    this->m_NeighbourList = oth.m_NeighbourList;
    this->m_labelCnts     = oth.m_labelCnts;
  }
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
  m_NeighbourList.clear();
  m_labelCnts.clear();
}

// Adjust the weights for the current node.
void AdjustWeights( std::vector<double> const& target,
                    double LearningRate, double Influence)
{
  target.size();
  std::cout << LearningRate << " " << Influence
            <<'\n';
}

/*
 * Set weights for the current node.
 *@param val The vector containing the weight for the current node.
 */
void PndSomNode::SetWeight(std::vector<float> const& val)
{
  this->m_Weights   = val;
  this->m_weightDim = val.size();
}

void PndSomNode::AddToRespList(size_t idx)
{
  (this->m_RespNodeIndex).push_back(idx);
}

std::vector<size_t> const& PndSomNode::GetRespoList() const
{
  return (this->m_RespNodeIndex);
}

/**
 * Set neigbours for the current node.
 *@param val The vector containing the neighbour indices.
 */
void PndSomNode::SetNeighbours(std::vector< std::pair<size_t, double> > const& val)
{
  if( (this->m_NeighbourList).size() != 0 )
  {
    (this->m_NeighbourList).clear();
  }
  this->m_NeighbourList = val;
}

void PndSomNode::SetRespList(std::vector<size_t> const& val)
{
  if( (this->m_RespNodeIndex).size() != 0 )
  {
    (this->m_RespNodeIndex).clear();
  }
  this->m_RespNodeIndex  = val;
}

/**
 * Removes all elements from the responsibility list.
*/
void PndSomNode::ResetRespList()
{
  (this->m_RespNodeIndex).clear();
}

/* Currently only on a rectangular grid. */
void PndSomNode::InitNode()
{
  // Calculate the node's center
  // m_xPos = left + (right - left)/2.00;
  // m_yPos = top  + (bot - top)/2.00;
  // m_xPos = ( m_iRight + m_iLeft )/2.00;
  // m_yPos = ( m_iBottom + m_iTop )/2.00;
  m_xPos = m_iLeft + (m_iRight  - m_iLeft)/2.00;
  m_yPos = m_iTop  + (m_iBottom - m_iTop )/2.00;
}

void PndSomNode::SetLabelMap(std::map<std::string, size_t> const &val)
{
  if( (this->m_labelCnts).size() != 0 )
  {
    (this->m_labelCnts).clear();
  }
  this->m_labelCnts = val;
}

std::map<std::string, size_t> const& PndSomNode::GetLabelMap() const
{
  return this->m_labelCnts;
}

///////__________ DEBUG FUNCTIONS _______________
#if (PRINT_PND_SOM_NODE_DEBUG > 0)
void PndSomNode::PrintNode() const
{
  std::cerr << "<DEBUG_INFO> Printing the info on the current node\n"
            << "\t m_weightDim = " << m_weightDim
            << " m_label = " << m_label
            << " length (m_Weights) = " << m_Weights.size()
            << " Lenght of reponsibility list = " << m_RespNodeIndex.size()
            << '\n';
  for( size_t w = 0; w < m_Weights.size(); ++w)
  {
    std::cerr << "[" << w << "] = " << m_Weights[w] << ", ";
  }
  std::cerr << '\n';

  // Print dist maps
#if (PRINT_PND_SOM_NODE_DEBUG > 2)
  std::cerr << "Neighbourhood list \n";
  for(size_t j = 0; j < m_NeighbourList.size(); ++j)
  {
    std::cerr << "(" << m_NeighbourList[j].first
              << ", " << m_NeighbourList[j].second
              << ") ";
  }
  std::cerr << '\n';
#endif

  // Print responsibility lists
#if (PRINT_PND_SOM_NODE_DEBUG > 2)
  std::cerr << "Index list = ";
  for(size_t j = 0; j < m_RespNodeIndex.size(); ++j)
  {
    std::cerr << m_RespNodeIndex[j] << ", ";
  }
  std::cerr << '\n';
#endif
}
#endif
///////__________ END DEBUG FUNCTIONS ____________

/* ********************************
 * Self Organizing Map node class *
 * Author: M.Babai@rug.nl         *
 * Version:                       *
 * LICENSE:                       *
 * ********************************
 */
#pragma once
#ifndef PND_SOM_NODE_H
#define PND_SOM_NODE_H

#include <vector>
#include <string>

#define PRINT_SOMNODE_DEBUG 0

class PndSomNode
{
 public:
  /**
   * Default constructor.
   */
  PndSomNode();

  /**
   * Constructor
   *@param WeightsDim Dimension of the weights.
   */
  explicit PndSomNode(size_t WeightsDim);

  /**
   * Constructor
   *@param weight Weights of the current node.
   */
  explicit PndSomNode(std::vector<float> const& weight);

  /**
   * Constructor
   *@param label Assigned label to the current node.
   *@param weight Weights of the current node.
   */
  PndSomNode( std::string const& label, std::vector<float> const& weight);

  /**
   * Constructor
   *@param lft Left neighbor.
   *@param top Top neighbor.
   *@param rgt Right neighbor.
   *@param bot Bottom neighbor.
   *@param WeightsDim Dimension of the weights.
   */
  PndSomNode(int lft, int top, int rgt, int bot, size_t WeightsDim = 0);

  /**
   * Constructor
   *@param lft Left neighbor.
   *@param top Top neighbor.
   *@param rgt Right neighbor.
   *@param bot Bottom neighbor.
   *@param WeightsDim Dimension of the weights.
   *@param label Assigned label to the current node.
   */
  PndSomNode(int lft, int top, int rgt, int bot, size_t WeightsDim,
             std::string const& label);
  
  /**
   * Constructor
   *@param lft Left neighbor.
   *@param top Top neighbor.
   *@param rgt Right neighbor.
   *@param bot Bottom neighbor.
   *@param label Assigned label to the current node.
   *@param weight Weights of the current node.
   */
  PndSomNode(int lft, int top, int rgt, int bot,
             std::string const& label, std::vector<float> const& weight);

  // Copy Constructor
  PndSomNode(PndSomNode const &oth);
  
  // Destructor
  virtual ~PndSomNode();

  // Operators
  // Assignment
  PndSomNode& operator=(PndSomNode const &oth);

  // Getter and setter
  inline int  GetLeft() const;
  inline void SetLeft(int val);

  inline int  GetRight() const;
  inline void SetRight(int val);

  inline int  GetTop() const;
  inline void SetTop(int val);

  inline int  GetBottom() const;
  inline void SetBottom(int val);

  inline size_t GetNodeDimension() const;
  inline void   SetNodeDimension( size_t val);

  inline std::string const& GetLabel() const;
  inline void SetLabel(std::string const& val);

  inline std::vector<float> const& GetWeight() const;
  inline void SetWeight(std::vector<float> const& val);

  inline double GetXPos() const;
  inline void   SetXPos( double xp);

  inline double GetYPos() const;
  inline void   SetYPos( double yp);

#if (PRINT_SOMNODE_DEBUG > 0)
  void PrintNode() const;
#endif
 
  // protected:

 private:
  //_______________ Functions ____________________
  // Clear the internal data structure (reset).
  void ClearInternalStructures();

  // Adjust the weights for the current node.
  void AdjustWeights( std::vector<double> const& target,
                      double LearningRate, double Influence);

  //___________________ Variables ___________________
  // Neighbor indices (2D for visualization)
  int  m_iLeft;
  int  m_iTop;
  int  m_iRight;
  int  m_iBottom;

  // Position of the node in the grid cell
  double m_xPos;
  double m_yPos;

  // Dimension of the weights
  size_t m_weightDim;

  // Label of the current node.
  std::string m_label;

  // Weight vector of the current node.
  std::vector<float> m_Weights;
  
  /* Indices of the actual data point which are under the
     responsibility of the current node.*/
  std::vector<size_t> m_RespNodeIndex;

  // Indices of te neighbors for the current map node.
  std::vector<size_t> m_NeighborList;
};
//______________ Inline functions ____________________
inline int PndSomNode::GetLeft() const
{
  return this->m_iLeft;
};

inline void PndSomNode::SetLeft(int val)
{
  this->m_iLeft = val;
};

inline int PndSomNode::GetRight() const
{
  return this->m_iRight;
};

inline void PndSomNode::SetRight(int val)
{
  this->m_iRight = val;
};

inline int PndSomNode::GetTop() const
{
  return this->m_iTop;
};

inline void PndSomNode::SetTop(int val)
{
  this->m_iTop = val;
};

inline int PndSomNode::GetBottom() const
{
  return this->m_iBottom;
};

inline void PndSomNode::SetBottom(int val)
{
  this->m_iBottom = val;
};

inline size_t PndSomNode::GetNodeDimension() const
{
  return this->m_weightDim;
};

inline void PndSomNode::SetNodeDimension(size_t val)
{
  this->m_weightDim = val;
};

inline std::string const& PndSomNode::GetLabel() const
{
  return this->m_label;
};

inline void PndSomNode::SetLabel(std::string const& val)
{
  this->m_label = val;
};

inline std::vector<float> const& PndSomNode::GetWeight() const
{
  return this->m_Weights;
};

inline void PndSomNode::SetWeight(std::vector<float> const& val)
{
  this->m_Weights = val;
};

inline double PndSomNode::GetXPos() const
{
  return this->m_xPos;
};

inline void PndSomNode::SetXPos( double xp)
{
  this->m_xPos = xp;
};

inline double PndSomNode::GetYPos() const
{
  return this->m_yPos;
};

inline void PndSomNode::SetYPos( double yp)
{
  this->m_yPos = yp;
};

#endif// End of interface

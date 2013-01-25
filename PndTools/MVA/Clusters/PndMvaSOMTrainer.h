/* ****************************************
 * Self Organizing Map Training functions *
 * Author: M.Babai@rug.nl                 *
 * Version:                               *
 * LICENSE:                               *
 * ****************************************
 */
#pragma once
#ifndef PND_SOM_TRAINER_H
#define PND_SOM_TRAINER_H

// C & C++
#include <vector>

// Local includes
class PndSomNode;

// ROOT & PANDAroot
class TRandom3;

//! Data structure of the space points and the map model vectors.
typedef std::vector< std::pair<std::string, std::vector<float>*> > DataPoints;

typedef enum MapNodeInitType{
  RAND_FROM_DATA = 0, // Select randomly from data vector.
  RANDOM         = 1  // Use random numbers
} MapNodeInitType;

typedef enum GridInitType{
  RECTANGULAR = 0, // Init rectangular grid
  HEXAGONAL   = 1  // Init hexagonal grid
} GridInitType;

// Debug constants
#define PRINT_SOMTRAIN_DEBUG_INFO 1

class PndMvaSomTrainer
{
  //----------------------------------------
  //================== public ==============
 public:

  /**
   * Constructor.
   *@param InputData Input Data points.
   *@param mapWidth The width of the SOM.
   *@param mapHeight The height of the SOM.
   *@param numIter  Number of learning iterations.
   *@param initType The scheme to initialize weight of each node.
   */
  explicit PndMvaSomTrainer( DataPoints const* const InputData,
                             size_t mapWidth, size_t mapHeight, size_t numIter,
                             MapNodeInitType initType = RAND_FROM_DATA,
                             GridInitType gridInitType = RECTANGULAR);
  /**
   * Destructor.
   */
  virtual ~PndMvaSomTrainer();

  /**
   * Initialize the map according to the given scheme.
   */
  void InitMap();

  /**
   * Train map using batch schema.
   *@return Vector containing the map model vectors.
   */
  virtual void TrainBatch();

  /**
   * Train the map using Online scheme.
   *@return Vector containing the map model vectors.
   */
  virtual void TrainOnline();

  //____________ Getters and Setters
  //________________________________________
  /**
   *@return The actual SOM.
   */
  inline std::vector<PndSomNode*> const& GetTheMap() const;

  /**
   * Get the input data set.
   *@return The list of input data points.
   */
  inline DataPoints const& GetInputDataSet() const;

  /**
   *@return The total number of map nodes.
   */
  inline size_t GetNumNodes() const;

  inline void SetSigmaZero(double val);
  inline double GetSigmaZero() const;

  inline void SetLambda(double val);
  inline double GetLambda() const;

  inline void SetNodeInitType(MapNodeInitType val = RAND_FROM_DATA);
  inline MapNodeInitType GetNodeInitType() const;

  inline size_t GetMapHeight() const;
  inline void   SetMapHeight(size_t val);

  inline size_t GetMapWidth()  const;
  inline void   SetMapWidth(size_t val);

  inline size_t GetNumIterations() const;
  inline void   SetNumIterations(size_t val);

  //--------------------------------------------
  //================= protected ================
 protected:

  //_____________ DEBUG _________________
#if (PRINT_SOMTRAIN_DEBUG_INFO > 0)
  void printMapGrid() const;
#endif

  //--------------------------------------------
  //================= private ==================
  
 private:
  //_______________ Functions ____________________
  // To avoid mistakes, :).
  PndMvaSomTrainer(PndMvaSomTrainer const& oth);/* Copy */
  PndMvaSomTrainer& operator=(PndMvaSomTrainer const& oth);/*Assign*/

  /*
   * Init a hexagonal grid.
   */
  void InitGridRectAngular();
  
  /*
   * Init a rectangular grid.
   */
  void InitGridHexagonal();
  
  /*
   * Initialize map nodes using random vectors fetched from the data
   * set.
   */
  void InitMap_RandomFromData();
  
  /*
   * Initialize map nodes using vectors with random numbers. 
   */
  void InitMap_Random();
  
  /*
   * Returns the index of the BMU map node.
   *
   *@param vector vector containing the coordinates of the current
   * data point.
   */
  size_t FindBestMatchingNode(std::vector<float> const& vector);
  
  //___________________ Variables ___________________
  double m_sigmaZero;//the width of the lattice at time t0
  double m_lambda;// A time constant, determine neighborhood

  size_t m_MapWidth;// Width of the map
  size_t m_MapHeight;// Height of the map
  size_t m_NumModelVectors;// Number of map nodes
  size_t m_NumIterations;//Number of iterations for learning.

  MapNodeInitType m_InitMode;// Init Scheme
  GridInitType    m_GridType;// Type of the map grid.
  std::vector<PndSomNode*> m_TheMap;// The actual map container
  DataPoints const* m_DataSet;// Data points used to train the map.
};// End of class definition

//---------------------- INLINE functions ____________
inline std::vector<PndSomNode*> const& PndMvaSomTrainer::GetTheMap() const
{
  return this->m_TheMap;
};

inline DataPoints const& PndMvaSomTrainer::GetInputDataSet() const
{
  return (*(this->m_DataSet));
};

inline void PndMvaSomTrainer::SetSigmaZero(double val)
{
  this->m_sigmaZero = val;
};

inline void PndMvaSomTrainer::SetLambda(double val)
{
  this->m_lambda = val;
};

inline void PndMvaSomTrainer::SetNodeInitType(MapNodeInitType val)
{
  this->m_InitMode = val;
};

inline double PndMvaSomTrainer::GetSigmaZero() const
{
  return this->m_sigmaZero;
};

inline double PndMvaSomTrainer::GetLambda() const
{
  return this->m_lambda;
};

inline MapNodeInitType PndMvaSomTrainer::GetNodeInitType() const
{
  return this->m_InitMode;
};

inline size_t PndMvaSomTrainer::GetMapHeight() const
{
  return this->m_MapHeight;
};

inline void PndMvaSomTrainer::SetMapHeight(size_t val)
{
  this->m_MapHeight = val;
};

inline size_t PndMvaSomTrainer::GetMapWidth()  const
{
  return this->m_MapWidth;
};

inline void PndMvaSomTrainer::SetMapWidth(size_t val)
{
  this->m_MapWidth = val;
};

inline size_t PndMvaSomTrainer::GetNumNodes() const
{
  return (this->m_MapWidth * this->m_MapHeight);
};

inline size_t PndMvaSomTrainer::GetNumIterations() const
{
  return this->m_NumIterations;
};

inline void   PndMvaSomTrainer::SetNumIterations(size_t val)
{
  this->m_NumIterations = val;
};
#endif// End of interface

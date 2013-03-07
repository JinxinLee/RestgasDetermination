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

//________________________
#include <vector>

//________________________
class PndSomNode;

//________________________
class TRandom3;

/**
 * Data structure of the space points and the map model vectors.
 */
typedef std::vector< std::pair<std::string, std::vector<float>*> > DataPoints;

/**
 * Scheme to initialize the model vector inside each of the map nodes.
 */
typedef enum MapNodeInitType{
  SOM_RAND_FROM_DATA = 0, // Select randomly from data vector.
  SOM_RANDOM         = 1  // Use random numbers
} MapNodeInitType;

/*
 * Scheme for the shape of the grid.
*/
typedef enum GridInitType{
  RECTANGULAR = 0, // Init rectangular grid
  HEXAGONAL   = 1  // Init hexagonal grid
} GridInitType;

// Debug constants, 0 = no DEBUG, 1 = DEBUG info
#define PRINT_PND_SOM_TRAIN_DEBUG_INFO 1

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
                             MapNodeInitType initType = SOM_RAND_FROM_DATA,
                             GridInitType gridInitType = RECTANGULAR);
  /**
   * Destructor.
   */
  virtual ~PndMvaSomTrainer();

  /**
   * Initialize the map according to the given scheme.
   */
  virtual void InitMap();
  
  /**
   * Train map using batch schema. All available data vectors are
   * presented at once.
   */
  virtual void TrainBatch();
  
  /**
   * Train the map using Online scheme. Data vectors are presented one
   * at a time.
   */
  virtual void TrainOnline();

  /**
   * Calibrate the map (post labeling). The current implementation
   * uses the winner takes all scheme.
   */
  virtual void Calibrate();

  //____________ Getters and Setters ______
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
  
  /**
   * Set initial value for sigma.
   */
  inline void SetSigmaZero(double val);
  inline double GetSigmaZero() const;
  
  /**
   * Set initial value for lambda.
   */
  inline void SetLambda(double val);
  inline double GetLambda() const;

  /**
   * Set initialization method.
   */
  inline void SetNodeInitType(MapNodeInitType val = SOM_RAND_FROM_DATA);
  inline MapNodeInitType GetNodeInitType() const;

  /**
   * Map size.
   */
  inline size_t GetMapHeight() const;
  inline void   SetMapHeight(size_t val);

  inline size_t GetMapWidth()  const;
  inline void   SetMapWidth(size_t val);

  /**
   * Number of iteration (epochs, learning steps).
   */
  inline size_t GetNumIterations() const;
  inline void   SetNumIterations(size_t val);

  //--------------------------------------------
  //================= protected ================
 protected:

  //_____________ DEBUG _________________
#if (PRINT_PND_SOM_TRAIN_DEBUG_INFO > 0)
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
   * Init a rectangular grid.
   */
  void InitGridRectAngular();
  
  /*
   * Init a hexagonal grid.
   */
  void InitGridHexagonal();
  
  /*
   * Initialize map nodes using random vectors fetched from the data
   * set.
   */
  void InitMapnodes_RandomFromData();
  
  /*
   * Initialize map nodes using vectors with random numbers. 
   */
  void InitMapnodes_Random();
  
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
  double m_neighbourhoodRadius; //the current width of area of influence
 
  size_t m_MapWidth;// Width of the map
  size_t m_MapHeight;// Height of the map
  size_t m_NumModelVectors;// Number of map nodes
  size_t m_NumIterations;//Number of iterations for learning.

  MapNodeInitType m_InitMode;// Init Scheme
  GridInitType    m_GridType;// Shape of the map grid.
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

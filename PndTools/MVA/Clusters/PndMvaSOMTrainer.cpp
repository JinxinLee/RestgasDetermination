/* ****************************************
 * Self Organizing Map Training functions *
 * Author: M.Babai@rug.nl                 *
 * Version:                               *
 * LICENSE:                               *
 * ****************************************
 */
///////////////////////////////
#include <iostream>
#include <limits>
#include <cassert>
#include <cmath>
#include <algorithm>

#include "TRandom3.h"

#include "PndMvaSOMTrainer.h"
#include "PndSomNode.h"
#include "PndMvaUtil.h"
/////////////////////////////////
/**
 * Constructor.
 *@param InputData Input Data points.
 *@param mapWidth The width of the SOM.
 *@param mapHeight The height of the SOM.
 *@param numIter  Number of learning iterations.
 *@param initType The scheme to initialize weight of each node.
 */
PndMvaSomTrainer::PndMvaSomTrainer( DataPoints const* const InputData,
                                    size_t mapWidth, size_t mapHeight,
                                    size_t numIter,
                                    MapNodeInitType initType,
                                    GridInitType gridInitType)
  : m_sigmaZero(0),
    m_lambda(0),
    m_MapWidth (mapWidth),
    m_MapHeight(mapHeight),
    m_NumModelVectors(m_MapWidth * mapHeight),
    m_NumIterations(numIter),
    m_InitMode(initType),
    m_GridType(gridInitType),
    m_TheMap(),
    m_DataSet(InputData)
{}

/**
 * Destructor.
 */
PndMvaSomTrainer::~PndMvaSomTrainer()
{
  for(size_t i = 0; i < m_TheMap.size(); ++i) {
    delete m_TheMap[i];
  }
  m_TheMap.clear();

  m_DataSet = 0;
}

/**
 * Initialize the map according to the given scheme.
 */
void PndMvaSomTrainer::InitMap()
{
  // Check if the data set is not empty.
  assert( m_DataSet->size() != 0 );

  // Determine the start radius.
  m_sigmaZero = static_cast <double> (std::max(m_MapWidth, m_MapHeight) )/2.0;

  assert ( m_sigmaZero > 0);
  m_lambda    = static_cast <double> (m_NumIterations) / log(m_sigmaZero);

  // Init the map grid.
  switch(m_GridType) {
  case HEXAGONAL:// Hexagonal grid
    std::cout << "\t<-I-> Init Hexagonal map grid.\n";
    InitGridHexagonal();
    break;
  case RECTANGULAR:// Rectangular grid.
  default:
    std::cout << "\t<-I-> Init Rectangular map grid.\n";
    InitGridRectAngular();
  }

  // Init per node weight vector.
  switch(m_InitMode) {
  case RANDOM:// Random numbers
    std::cout << "\t<-I-> Init map nodes using Random numbers.\n";
    InitMap_Random();
    break;
  case RAND_FROM_DATA:// Random vectors selected from data set
  default:
    std::cout << "\t<-I-> Init map nodes using Points from data set.\n";
    InitMap_RandomFromData();
  }
}

/*
 * Init a rectangular grid.
 */
void PndMvaSomTrainer::InitGridRectAngular()
{
  int lfn, rtn, tpn, btn;

  lfn = -1;
  for(size_t r = 0; r < m_MapHeight; ++r) {// rows
    tpn = r - 1;
    btn = ( (r + 1) < m_MapHeight )? (r + 1) : -1;
    for(size_t c = 0; c < m_MapWidth; ++c) {// columns
      rtn = ((lfn + 2) < static_cast<int>(m_MapWidth) )? (lfn + 2) : -1;
      PndSomNode* node = new PndSomNode(lfn, tpn, rtn, btn);
      // Add the node
      m_TheMap.push_back(node);
      lfn++;
    }
    lfn = -1;
  }
  // Debug info
#if (PRINT_SOMTRAIN_DEBUG_INFO > 0)
  printMapGrid();
#endif
}

/*
 * Init a hexagonal grid.
 */
void PndMvaSomTrainer::InitGridHexagonal()
{
  std::cerr << "\n\n<ERROR> NOT Implemented yet\n\n";
}

/*
 * Initialize map nodes using random vectors fetched from the data
 * set.
 */
void PndMvaSomTrainer::InitMap_RandomFromData()
{
  // Fetch random examples from the data set.
  TRandom3 rnd;
  size_t index;
  
  for( size_t i = 0; i < m_NumModelVectors; ++i) {
    // Select a Random index
    index = static_cast<size_t>(rnd.Uniform(0, (m_DataSet->size() -1 )));
    // Fetch point
    std::vector<float> const* cur_example = (m_DataSet->at(index)).second;
    // Modify the node.
    PndSomNode *node = m_TheMap[i];
    node->SetWeight(*cur_example);
  }
#if (PRINT_SOMTRAIN_DEBUG_INFO > 0)
  std::cout << " Number of model vectors is " << m_TheMap.size()
            << '\n';
#endif
}

/*
 * Initialize map nodes using vectors with random numbers. 
 */
void PndMvaSomTrainer::InitMap_Random()
{
  // Non empty data set.
  assert(m_DataSet->size() != 0);

  // If the map nodes are initialized
  assert (m_TheMap.size() == m_NumModelVectors);

  // Find the vector length
  size_t dimension = ( (m_DataSet->at(0)).second)->size();
  TRandom3 rnd;

  // Map node loop.
  for( size_t i = 0; i < m_NumModelVectors; ++i) {
    std::vector<float> weight(dimension);
    // Dimension loop (init vector components).
    for(size_t d = 0; d < dimension; ++d) {
      weight[d] = static_cast<float>(rnd.Uniform());
    }
    // Modify the node.
    PndSomNode *node = m_TheMap[i];
    node->SetWeight(weight);
  }
#if (PRINT_SOMTRAIN_DEBUG_INFO > 0)
  std::cout << " Number of model vectors is " << m_TheMap.size()
            << '\n';
#endif
}

/*
 *returns the index of the BMU map node.
 *
 *@param vector vector containing the coordinates of the current
 * data point.
 */
size_t PndMvaSomTrainer::FindBestMatchingNode(std::vector<float> const& vec)
{
  float min_dist = std::numeric_limits<float>::max();
  float cur_dist = 0.0;
  size_t mbu_index = 0;

  // Map node loop.
  for( size_t i = 0; i < m_NumModelVectors; ++i) {
    PndSomNode const* cur_node = m_TheMap[i];
    std::vector<float> const& cur_weights = cur_node->GetWeight();
    // Compute the distance to the current Map node.
    cur_dist = ComputeDist(cur_weights, vec);
    // Find the index of BMU
    if(cur_dist < min_dist) {
      min_dist = cur_dist;
      mbu_index = i;
    }
  }
  return mbu_index;
}

/**
 * Train map using batch schema.
 *@return Vector containing the map model vectors.
 */
void PndMvaSomTrainer::TrainBatch()
{}

/**
 * Train the map using Online scheme.
 *@return Vector containing the map model vectors.
 */
void PndMvaSomTrainer::TrainOnline()
{}

//_____________ DEBUG _________________
#if (PRINT_SOMTRAIN_DEBUG_INFO > 0)
void PndMvaSomTrainer::printMapGrid() const
{
  for(size_t i = 0; i < m_TheMap.size(); ++i) {
    PndSomNode const* node = m_TheMap[i];
    std::cout << "<DEBUG> left = " << node->GetLeft()
              << " right = " << node->GetRight()
              << " top = " << node->GetTop()
              << " bottom = " << node->GetBottom()
              << " X-pos = " << node->GetXPos()
              << " Y-pos = " << node->GetYPos()
              << '\n';
  }
}
#endif

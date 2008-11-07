// simple soliton interface to manage random number generation

class TRandom3;

class PndFsmRandom {
public:
  static TRandom3* Instance();
private:
  PndFsmRandom() {};
  ~PndFsmRandom() {};
  static TRandom3* frand;
};

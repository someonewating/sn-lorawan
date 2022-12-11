
class Resume {
public:
  Resume();
  void add_value(double value);
  double get_mean();
  double get_max();
  double get_min();

private:
  double _sum;
  long _counter;
  double _max_value;
  double _min_value;
};
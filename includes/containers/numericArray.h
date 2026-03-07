#include <array>
#include <vector>

template<typename T, std::size_t N>
class NumericArray {
private:
    std::array<T, N> _data;

public:
    T& operator[](std::size_t i){ return _data[i]; }
    const T& operator[](std::size_t i){return _data[i]; }

    NumericArray operator+(const NumericArray& b){
        NumericArray x;
        for(int i = 0; i < N; i++){
            x[i] = _data[i] + b[i];
        }
        return x;
    }

    NumericArray operator-(const NumericArray& b){
        NumericArray x;
        for(int i = 0; i < N; i++){
            x[i] = _data[i] - b[i];
        }
        return x;
    }

    T sum(){
        T val{};
        for(int i = 0; i < N; i++){
            val += _data[i];
        }
        return val;
    }

    void normalize(){
        T val = sum();
        for(int i = 0; i < N; i++){
            _data[i] /= val;
        }
    }
    NumericArray cum_sum(){
        NumericArray c;
        T val{};
        for(int i = 0; i < N; i++){
            val += _data[i];
            c[i] = val;
        }
        return c;
    }
};
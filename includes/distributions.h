#include <array>
#include <vector>

template<typename T, std::size_t N>
struct Distribution {
    std::array<T, N> _data;

    T& operator[](std::size_t i){ return _data[i]; }
    const T& operator[](std::size_t i){return _data[i]}

    Distribution operator+(const Distribution& b){
        Distribution x;
        for(int i = 0; i < N; i++){
            x[i] = _data[i] + b[i];
        }
        return x;
    }

    Distribution operator-(const Distribution& b){
        Distribution x;
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
    T abs_diff(const Distribution<T, N>& x, const Distribution<T, N>& y) {
        Distribution val;
        for (std::size_t i = 0; i < N; ++i)
            val[i] = std::abs(x[i] - y[i]);
        return val;
    }

    void normalize(){
        T val = sum();
        for(int i = 0; i < N; i++){
            _data[i] /= val;
        }
    }

    Distribution cum_sum(){
        Distribution c;
        T val{};
        for(int i = 0; i < N; i++){
            val += _data[i];
            c[i] = val;
        }
        return c;
    }

};
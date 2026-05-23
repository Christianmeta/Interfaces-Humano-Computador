#include <iostream>
#include <vector>
#include <iomanip>

using namespace std;

class LeastSquaredRegressor {
private:
    vector<vector<double>> X;
    vector<double> y;
    vector<double> beta; //almacenara B0, B1, B2

    //metodos privados para operaciones matriciales

    //transpuesta de una matriz
    vector<vector<double>> transpose(const vector<vector<double>>& matrix) {
        int rows = matrix.size();
        int cols = matrix[0].size();
        vector<vector<double>> transposed(cols, vector<double>(rows));
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                transposed[j][i] = matrix[i][j];
            }
        }
        return transposed;
    }

    //multiplicacion de dos matrices
    vector<vector<double>> multiplyMatrix(const vector<vector<double>>& A, const vector<vector<double>>& B) {
        int rA = A.size();
        int cA = A[0].size();
        int cB = B[0].size();
        vector<vector<double>> result(rA, vector<double>(cB, 0.0));
        for (int i = 0; i < rA; ++i) {
            for (int j = 0; j < cB; ++j) {
                for (int k = 0; k < cA; ++k) {
                    result[i][j] += A[i][k] * B[k][j];
                }
            }
        }
        return result;
    }

    //multiplicacion de matriz por vector
    vector<double> multiplyMatrixVector(const vector<vector<double>>& A, const vector<double>& V) {
        int rA = A.size();
        int cA = A[0].size();
        vector<double> result(rA, 0.0);
        for (int i = 0; i < rA; ++i) {
            for (int k = 0; k < cA; ++k) {
                result[i] += A[i][k] * V[k];
            }
        }
        return result;
    }

    //inversa de una matriz 3x3 por el metodo de la adjunta y determinante
    vector<vector<double>> inverse3x3(const vector<vector<double>>& m) {
        double det = m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2]) -
                     m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
                     m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
                     
        vector<vector<double>> inv(3, vector<double>(3));
        inv[0][0] = (m[1][1] * m[2][2] - m[2][1] * m[1][2]) / det;
        inv[0][1] = (m[0][2] * m[2][1] - m[0][1] * m[2][2]) / det;
        inv[0][2] = (m[0][1] * m[1][2] - m[0][2] * m[1][1]) / det;
        inv[1][0] = (m[1][2] * m[2][0] - m[1][0] * m[2][2]) / det;
        inv[1][1] = (m[0][0] * m[2][2] - m[0][2] * m[2][0]) / det;
        inv[1][2] = (m[1][0] * m[0][2] - m[0][0] * m[1][2]) / det;
        inv[2][0] = (m[1][0] * m[2][1] - m[2][0] * m[1][1]) / det;
        inv[2][1] = (m[2][0] * m[0][1] - m[0][0] * m[2][1]) / det;
        inv[2][2] = (m[0][0] * m[1][1] - m[1][0] * m[0][1]) / det;
        return inv;
    }

public:
    //constructor: predefine el dataset y construye las matrices X e y
    LeastSquaredRegressor() {
        double dataX[17][2] = {
            {41.9, 29.1}, {43.4, 29.3}, {43.9, 29.5}, {44.5, 29.7},
            {47.3, 29.9}, {47.5, 30.3}, {47.9, 30.5}, {50.2, 30.7},
            {52.8, 30.8}, {53.2, 30.9}, {56.7, 31.5}, {57.0, 31.7},
            {63.5, 31.9}, {65.3, 32.0}, {71.1, 32.1}, {77.0, 32.5},
            {77.8, 32.9}
        };
        double dataY[17] = {
            251.3, 251.3, 248.3, 267.5, 273.0, 276.5, 270.3, 274.9,
            285.0, 290.0, 297.0, 302.5, 304.5, 309.3, 321.7, 330.7, 349.0
        };

        //llenar matriz X agregando la columna de 1s para B0 y vector y
        for (int i = 0; i < 17; ++i) {
            X.push_back({1.0, dataX[i][0], dataX[i][1]});
            y.push_back(dataY[i]);
        }
        beta = {0, 0, 0};
    }

    //calcula B = (X^T * X)^-1 * X^T * y
    void fit() {
        vector<vector<double>> XT = transpose(X);
        vector<vector<double>> XTX = multiplyMatrix(XT, X);
        vector<vector<double>> XTX_inv = inverse3x3(XTX);
        vector<double> XTY = multiplyMatrixVector(XT, y);
        
        beta = multiplyMatrixVector(XTX_inv, XTY);
    }

    //imprime la ecuacion sustituyendo los parametros calculados
    void printEquation() {
        cout << "--- Parametros Optimos Calculados ---" << endl;
        cout << fixed << setprecision(2);
        cout << "B_0 (Intercepto) : " << beta[0] << endl;
        cout << "B_1 (Pendiente x1): " << beta[1] << endl;
        cout << "B_2 (Pendiente x2): " << beta[2] << endl;
        cout << "\n--- Ecuacion de Regresion Polinomial (Multiple) ---" << endl;
        cout << "Yield = " << beta[0] << " + (" << beta[1] << " * Factor_1) + (" << beta[2] << " * Factor_2)\n" << endl;
    }

    //simula experimentos con valores desconocidos
    void simulate(double x1, double x2) {
        double prediction = beta[0] + (beta[1] * x1) + (beta[2] * x2);
        cout << "Simulacion para Factor 1 (" << x1 << ") y Factor 2 (" << x2 << ") -> Yield predicho: " << prediction << endl;
    }
};

int main() {
    //1 instanciar el objeto el dataset se carga en el constructor
    LeastSquaredRegressor model;

    //2 ejecutar el cálculo matricial para ajustar el modelo
    model.fit();

    //3 imprimir los resultados
    model.printEquation();

    //4 simulacion de 5 experimentos con valores desconocidos
    cout << "--- Simulacion de 5 Experimentos Desconocidos ---" << endl;
    model.simulate(45.0, 30.0);
    model.simulate(60.0, 31.0);
    model.simulate(80.0, 33.5);
    model.simulate(40.0, 28.5);
    model.simulate(55.5, 31.2);

    return 0;
}

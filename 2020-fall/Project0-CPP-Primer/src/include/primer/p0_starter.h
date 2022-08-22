//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// p0_starter.h
//
// Identification: src/include/primer/p0_starter.h
//
// Copyright (c) 2015-2020, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <utility>
#include "../common/logger.h"

namespace bustub {

/*
 * The base class defining a Matrix
 */
template <typename T>
class Matrix {
 protected:
  // TODO(P0): Add implementation
  Matrix(int r, int c) : rows(r), cols(c), linear(new T[r * c]) {}
  // # of rows in the matrix
  int rows;
  // # of Columns in the matrix
  int cols;
  // Flattened array containing the elements of the matrix
  // TODO(P0) : Allocate the array in the constructor. Don't forget to free up
  // the array in the destructor.
  T *linear;

 public:
  // Return the # of rows in the matrix
  virtual int GetRows() = 0;

  // Return the # of columns in the matrix
  virtual int GetColumns() = 0;

  // Return the (i,j)th  matrix element
  virtual T GetElem(int i, int j) = 0;

  // Sets the (i,j)th  matrix element to val
  virtual void SetElem(int i, int j, T val) = 0;

  // Sets the matrix elements based on the array arr
  virtual void MatImport(T *arr) = 0;

  // TODO(P0): Add implementation
  virtual ~Matrix() {
    delete[] this->linear;
    this->linear = nullptr;
    this->rows = 0;
    this->cols = 0;
  }
};

template <typename T>
class RowMatrix : public Matrix<T> {
 public:
  // TODO(P0): Add implementation
  RowMatrix(int r, int c) : Matrix<T>(r, c), data_(new T *[r]) {
    for (int i = 0; i < r; ++i) {
      this->data_[i] = this->linear + i * c;
    }
  }

  // TODO(P0): Add implementation
  int GetRows() override { return this->rows; }

  // TODO(P0): Add implementation
  int GetColumns() override { return this->cols; }

  // TODO(P0): Add implementation
  T GetElem(int i, int j) override { return this->data_[i][j]; }

  // TODO(P0): Add implementation
  void SetElem(int i, int j, T val) override { this->data_[i][j] = val; }

  // TODO(P0): Add implementation
  void MatImport(T *arr) override {
    for (int r = 0; r < this->rows; ++r) {
      for (int c = 0; c < this->cols; ++c) {
        this->data_[r][c] = arr[r * this->cols + c];
      }
    }
  }

  // TODO(P0): Add implementation
  ~RowMatrix() override {
    delete[] this->data_;
    this->data_ = nullptr;
  }

 private:
  // 2D array containing the elements of the matrix in row-major format
  // TODO(P0): Allocate the array of row pointers in the constructor. Use these pointers
  // to point to corresponding elements of the 'linear' array.
  // Don't forget to free up the array in the destructor.
  T **data_;
};

template <typename T>
class RowMatrixOperations {
 public:
  // Compute (mat1 + mat2) and return the result.
  // Return nullptr if dimensions mismatch for input matrices.
  static std::unique_ptr<RowMatrix<T>> AddMatrices(std::unique_ptr<RowMatrix<T>> mat1,
                                                   std::unique_ptr<RowMatrix<T>> mat2) {
    // TODO(P0): Add code
    if (mat1 == nullptr || mat2 == nullptr) { return nullptr; }

    if (!(mat1->GetRows() == mat2->GetRows() && mat1->GetColumns() == mat2->GetColumns())) { return nullptr; }

    std::unique_ptr<RowMatrix<T>> res = std::make_unique<RowMatrix<T>>(mat1->GetRows(), mat1->GetColumns());
    for (int r = 0; r < res->GetRows(); ++r) {
      for (int c = 0; c < res->GetColumns(); ++c) {
        res->SetElem(r, c, mat1->GetElem(r, c) + mat2->GetElem(r, c));
      }
    }
    return res;
  }

  // Compute matrix multiplication (mat1 * mat2) and return the result.
  // Return nullptr if dimensions mismatch for input matrices.
  static std::unique_ptr<RowMatrix<T>> MultiplyMatrices(std::unique_ptr<RowMatrix<T>> mat1,
                                                        std::unique_ptr<RowMatrix<T>> mat2) {
    // TODO(P0): Add code
    if (mat1 == nullptr || mat2 == nullptr) { return nullptr; }

    if (mat1->GetColumns() != mat2->GetRows()) { return nullptr; }

    std::unique_ptr<RowMatrix<T>> res = std::make_unique<RowMatrix<T>>(mat1->GetRows(), mat2->GetColumns());
    for (int r = 0; r < res->GetRows(); ++r) {
      for (int c = 0; c < res->GetColumns(); ++c) {
        int sum = 0;
        for (int k = 0; k < mat1->GetColumns(); ++k) {
          sum += mat1->GetElem(r, k) * mat2->GetElem(k, c);
        }
        res->SetElem(r, c, sum);
      }
    }
    return res;
  }

  // Simplified GEMM (general matrix multiply) operation
  // Compute (matA * matB + matC). Return nullptr if dimensions mismatch for input matrices
  static std::unique_ptr<RowMatrix<T>> GemmMatrices(std::unique_ptr<RowMatrix<T>> matA,
                                                    std::unique_ptr<RowMatrix<T>> matB,
                                                    std::unique_ptr<RowMatrix<T>> matC) {
    // TODO(P0): Add code
    return AddMatrices(MultiplyMatrices(std::move(matA), std::move(matB)), std::move(matC));
  }
};
}  // namespace bustub

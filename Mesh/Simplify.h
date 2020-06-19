#pragma once
#include <QMatrix4x4>
#include <Eigen/Geometry>

class GLModel;
namespace Hix
{
	namespace Engine3D
	{
		class Mesh;

		namespace Simplify
		{
			// Casting an std::vector<> from one type to another type without warnings about a loss of accuracy.
			template<typename T_TO, typename T_FROM>
			std::vector<T_TO> cast(const std::vector<T_FROM>& src)
			{
				std::vector<T_TO> dst;
				dst.reserve(src.size());
				for (const T_FROM& a : src)
					dst.emplace_back((T_TO)a);
				return dst;
			}

			template <typename MatrixType>
			Eigen::Matrix<MatrixType, 3, 1, Eigen::DontAlign> to_eigen_matrix(const QVector3D pos)
			{
				Eigen::Matrix<MatrixType, 3, 1, Eigen::DontAlign> result_matrix;
				result_matrix[0] = (MatrixType)(pos.x());
				result_matrix[1] = (MatrixType)(pos.y());
				result_matrix[2] = (MatrixType)(pos.z());

				return result_matrix;
			}

			void to_eigen_mesh(const Mesh* mesh, Eigen::MatrixXd& V, Eigen::MatrixXi& F);
			bool simlify_mesh(Eigen::MatrixXd& OV, Eigen::MatrixXi& OF);
			Mesh* to_hix_mesh(Mesh* mesh, Eigen::MatrixXd& OV, Eigen::MatrixXi& OF);
		}
	}
}
#pragma once
#include "layer_base.hpp"
#include "variable.hpp"

namespace nnk
{

	template <class T>
	class linear_layer : public layer_base
	{
	public:

		typedef T scalar_type;
		typedef std::valarray<scalar_type> tensor_type;

		linear_layer(
			std::size_t input_dimension,
			std::size_t output_dimension,
			scalar_type weight_scale = static_cast<scalar_type>(1.0))
			: input_dimension_(input_dimension)
			, output_dimension_(output_dimension)
		{
			BOOST_ASSERT(input_dimension >= 1);
			BOOST_ASSERT(output_dimension >= 1);

			std::mt19937 engine;
			std::normal_distribution<scalar_type> distribution(
				static_cast<scalar_type>(0.0),
				static_cast<scalar_type>(weight_scale * std::sqrt(1.0 / input_dimension)));

			weight_.value() = tensor_type(output_dimension * input_dimension);

			for (std::size_t i = 0; i < weight_.value().size(); ++i)
				weight_.value()[i] = distribution(engine);

			bias_.value() = tensor_type(output_dimension);
		}

		virtual void save(binary_writer& writer) const override
		{
			writer.write(input_dimension_);
			writer.write(output_dimension_);
			weight_.save(writer);
			bias_.save(writer);
		}

		virtual void load(binary_reader& reader) override
		{
			reader.read(input_dimension_);
			reader.read(output_dimension_);
			weight_.load(reader);
			bias_.load(reader);
		}

		virtual void enumerate_parameters(optimizer_base& optimizer) override
		{
			optimizer.add_parameter(weight_);
			optimizer.add_parameter(bias_);
		}

		std::size_t input_dimension() const
		{
			return input_dimension_;
		}

		std::size_t output_dimension() const
		{
			return output_dimension_;
		}

		variable<scalar_type>& weight()
		{
			return weight_;
		}

		variable<scalar_type>& bias()
		{
			return bias_;
		}

		expression<scalar_type> forward(const expression<scalar_type>& v) const
		{
			return matrix_product(weight_.expr(), v, output_dimension_, input_dimension_, input_dimension_, 1) + bias_.expr();
		}

	private:

		std::size_t input_dimension_;
		std::size_t output_dimension_;

		variable<scalar_type> weight_;
		variable<scalar_type> bias_;

	};

}

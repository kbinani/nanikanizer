#pragma once
#include "expression.hpp"
#include "expression_node.hpp"

namespace nnk
{

	template <class T>
	class sigmoid_cross_entropy_expression_node : public expression_node<T>
	{
	private:

		typedef expression_node<T> base_type;

	public:

		typedef typename base_type::scalar_type scalar_type;
		typedef typename base_type::tensor_type tensor_type;
		typedef typename base_type::node_pointer node_pointer;

		explicit sigmoid_cross_entropy_expression_node(const node_pointer& base)
			: base_(base)
		{
		}

		virtual bool is_branch() override
		{
			return true;
		}

		virtual void forward() override
		{
			if (this->output().size() != base_->output().size())
				this->output().resize(base_->output().size());

			for (std::size_t i = 0; i < base_->output().size(); ++i)
			{
				double x = base_->output()[i];
				double& y = this->output()[i];

				if (x < static_cast<scalar_type>(0.0))
					y = -std::log1p(std::exp(x));
				else
					y = -std::log1p(std::exp(-x));
			}
		}

		virtual void backward() override
		{
			for (std::size_t i = 0; i < this->output_grad().size(); ++i)
			{
				double x = base_->output()[i];
				double dy = this->output_grad()[i];
				double& dx = base_->output_grad()[i];

				if (x < static_cast<scalar_type>(0.0))
					dx += dy * (sigmoid(-x) - static_cast<scalar_type>(1.0));
				else
					dx += dy * sigmoid(-x);
			}
		}

		virtual void enumerate_children(const std::function<void(expression_node_base*)>& callback) override
		{
			callback(base_.get());
		}

	private:

		node_pointer base_;

	};

	template <class T>
	expression<T> sigmoid_cross_entropy(const expression<T>& base)
	{
		return expression<T>(std::make_shared<sigmoid_cross_entropy_expression_node<T>>(base.root()));
	}

}
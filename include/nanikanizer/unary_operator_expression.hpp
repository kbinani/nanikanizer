#pragma once
#include "expression.hpp"
#include "expression_node.hpp"

namespace nnk
{

	template <class T, class OP>
	class unary_operator_expression_node : public expression_node<T>
	{
	private:

		typedef expression_node<T> base_type;

	public:

		typedef typename base_type::scalar_type scalar_type;
		typedef typename base_type::tensor_type tensor_type;
		typedef typename base_type::node_pointer node_pointer;

		unary_operator_expression_node(const node_pointer& base)
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
				this->output()[i] = OP::forward(base_->output()[i]);
		}

		virtual void backward() override
		{
			for (std::size_t i = 0; i < this->output_grad().size(); ++i)
			{
				OP::backward(
					base_->output_grad()[i],
					base_->output()[i],
					this->output()[i],
					this->output_grad()[i]);
			}
		}

		virtual void enumerate_children(const std::function<void(expression_node_base*)>& callback) override
		{
			callback(base_.get());
		}

	private:

		node_pointer base_;

	};

	template <class OP, class T>
	expression<T> unary_operator(const expression<T>& base)
	{
		return expression<T>(std::make_shared<unary_operator_expression_node<T, OP>>(base.root()));
	}

}

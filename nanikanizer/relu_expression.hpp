#pragma once
#include "expression.hpp"
#include "expression_node.hpp"
#include "math_util.hpp"

namespace nnk
{

	template <class T>
	class relu_expression_node : public expression_node<T>
	{
	private:

		typedef expression_node<T> base;

	public:

		typedef typename base::scalar_type scalar_type;
		typedef typename base::tensor_type tensor_type;
		typedef typename base::node_pointer node_pointer;

		explicit relu_expression_node(const node_pointer& base)
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
				this->output()[i] = relu(base_->output()[i]);
		}

		virtual void backward() override
		{
			for (std::size_t i = 0; i < this->output_grad().size(); ++i)
				base_->output_grad()[i] += this->output_grad()[i] * relu_diff(this->output()[i]);
		}

		virtual void enumerate_children(const std::function<void(expression_node_base*)>& callback) override
		{
			callback(base_.get());
		}

	private:

		node_pointer base_;

	};

	template <class T>
	expression<T> relu(const expression<T>& base)
	{
		return expression<T>(std::make_shared<relu_expression_node<T>>(base.root()));
	}

}

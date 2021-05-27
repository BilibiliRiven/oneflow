/*
Copyright 2020 The OneFlow Authors. All rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#include "oneflow/core/framework/op_expr_grad_function.h"
#include "oneflow/core/framework/op_builder.h"
#include "oneflow/core/framework/op_expr.h"
#include "oneflow/core/framework/op_expr_helper.h"
#include "oneflow/core/framework/op_interpreter/op_interpreter_util.h"
#include "oneflow/core/framework/user_op_conf_trait.h"

namespace oneflow {
namespace one {

struct ConcatInterpState : public OpExprInterpState {
  bool requires_grad;
};

class Concat : public OpExprGradFunction<ConcatInterpState> {
 public:
  Maybe<void> Init(const OpExpr& op) override;
  Maybe<void> Capture(ConcatInterpState* ctx, const TensorTuple& inputs, const TensorTuple& outputs,
                      const AttrMap& attrs) const override;
  Maybe<void> Apply(const ConcatInterpState* ctx, const TensorTuple& out_grads,
                    TensorTuple* in_grads) const override;

 private:
  std::shared_ptr<user_op::UserOpConfTrait> op_trait_;
  AttrMap base_attrs_;
  std::shared_ptr<OpExpr> grad_op_;
  int64_t axis_;
};

Maybe<void> Concat::Init(const OpExpr& op) {
  const UserOpExpr* fw_op_expr = dynamic_cast<const UserOpExpr*>(&op);
  CHECK_NOTNULL_OR_RETURN(fw_op_expr);
  base_attrs_ = MakeAttrMapFromUserOpConf(fw_op_expr->proto());
  const std::string& op_name = fw_op_expr->op_name();
  op_trait_ = std::make_shared<user_op::UserOpConfTrait>(op_name, fw_op_expr->proto());
  axis_ = JUST(op_trait_->GetAttr<int64_t>("axis"));
  int32_t input_num = JUST(op_trait_->input_size("in"));
  grad_op_ = JUST(op_expr_helper::SplitLikeOp(input_num, axis_, GradientOpName(op_name)));
  return Maybe<void>::Ok();
}

Maybe<void> Concat::Capture(ConcatInterpState* ctx, const TensorTuple& inputs,
                            const TensorTuple& outputs, const AttrMap& attrs) const {
  int input_len = inputs.size();
  for (int i = 0; i < input_len; i++) {
    ctx->requires_grad = ctx->requires_grad | inputs.at(i)->requires_grad();
  }
  if (!ctx->requires_grad) { return Maybe<void>::Ok(); }

  ComposedAttrMap composed_attrs(attrs, base_attrs_);
  for (int i = 0; i < input_len; i++) { ctx->SaveTensorForBackward(inputs.at(i)); }
  return Maybe<void>::Ok();
}

Maybe<void> Concat::Apply(const ConcatInterpState* ctx, const TensorTuple& out_grads,
                          TensorTuple* in_grads) const {
  printf("enter concat apply");
  if (!ctx->requires_grad) { return Maybe<void>::Ok(); }
  //   CHECK_EQ_OR_RETURN(out_grads.size(), 1);
  auto &in = out_grads.at(0);
  int n = (*in_grads).size();
  TensorTuple like;
  like.reserve((*in_grads).size());
  for(int i = 0; i < n; i++){
    like.push_back(ctx->SavedTensors().at(i));
  }
  MutableAttrMap concat_attrs;
  concat_attrs.SetAttr<int>("axis", axis_);
  const auto& grads = JUST(OpInterpUtil::Dispatch<TensorTuple>(*grad_op_, {in, like}, concat_attrs));

  return Maybe<void>::Ok();
}

REGISTER_OP_EXPR_GRAD_FUNCTION("concat", Concat);

}  // namespace one
}  // namespace oneflow

#!/bin/bash

# 默认为 push
action="push"

# 如果传入了参数，则使用传入的参数
if [[ $# -gt 0 ]]; then
  action="$1"
fi

if [[ "$action" == "pull" ]]; then
  # 执行 pull 操作
  git branch | grep "lab" | xargs -r -I {} git fetch lab {}:{}
elif [[ "$action" == "push" ]]; then
  # 执行 push 操作
  git branch | grep "lab" | xargs -r git push lab
else
  # 参数错误
  echo "错误：无效的参数 $action"
  exit 1
fi

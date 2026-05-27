---
name: code-reviewer
description: 用于代码审查，检查代码质量

# code reviewer skill

## 职责
1. 检查代码是否符合规范
2. 识别潜在的安全漏洞
3. 建议性能优化方案

## 工作流程
1. 使用git diff获取变更文件列表
2. 对每个文件分析代码
3. 对照.claude/rules/style.md中的规范进行检查
4. 生成结构化审查报告

## 限制
1. 只针对ext和src目录进行检查
---

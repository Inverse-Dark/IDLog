/**
 * @Description: 过滤器基类头文件
 * @Author: InverseDark
 * @Date: 2025-12-18 20:36:25
 * @LastEditTime: 2025-12-18 20:37:38
 * @LastEditors: InverseDark
 */
#ifndef IDLOG_FILTER_FILTER_H
#define IDLOG_FILTER_FILTER_H

#include "IDLog/Core/LogEvent.h"

#include <vector>

namespace IDLog
{
	/// @brief 过滤器决策
	enum class FilterDecision
	{
		ACCEPT, ///< 接受日志事件
		DENY,	///< 拒绝日志事件
		NEUTRAL ///< 中立，由后续过滤器决定
	};

	/// @brief 过滤器基类
	/// @details 提供比日志级别更精细的日志控制
	///			 支持链式过滤器
	class IDLOG_API Filter
	{
	public:
		using Pointer = std::shared_ptr<Filter>;
		using LogEventPtr = LogEvent::Pointer;

	public:
		/// @brief 析构函数
		virtual ~Filter() = default;

		/// @brief 过滤日志事件
		/// @param event [IN] 日志事件智能指针
		/// @return 过滤决策
		virtual FilterDecision Decide(const LogEventPtr &event) = 0;

		/// @brief 获取过滤器名称
		/// @return 过滤器名称
		virtual std::string GetName() const = 0;

		/// @brief 克隆过滤器
		/// @return 过滤器智能指针
		virtual Pointer Clone() const = 0;

		/// @brief 添加子过滤器
		/// @param filter [IN] 过滤器智能指针
		virtual void AddFilter(const Pointer &filter);

		/// @brief 获取所有子过滤器
		/// @return 子过滤器列表
		virtual std::vector<Pointer> GetFilters() const;

		/// @brief 清空子过滤器
		virtual void ClearFilters();
	};

	/// @brief 组合过滤器
	class IDLOG_API CompositeFilter : public Filter
	{
	public:
		/// @brief 构造函数
		CompositeFilter();
		/// @brief 析构函数
		virtual ~CompositeFilter();

		/// @brief 过滤日志事件
		/// @param event [IN] 日志事件智能指针
		/// @return 过滤决策
		FilterDecision Decide(const LogEventPtr &event) override;

		/// @brief 添加子过滤器
		/// @param filter [IN] 过滤器智能指针
		void AddFilter(const Pointer &filter) override;

		/// @brief 获取所有子过滤器
		/// @return 子过滤器列表
		std::vector<Pointer> GetFilters() const override;

		/// @brief 清空子过滤器
		void ClearFilters() override;

	protected:
		/// @brief 组合过滤器实现结构体前向声明
		struct Impl;
	protected:
		Impl* m_pImpl; ///< 组合过滤器实现指针
	};

	/// @brief 与过滤器（所有子过滤器都接受时才接受）
	class IDLOG_API AndFilter : public CompositeFilter
	{
	public:
		/// @brief 过滤日志事件
		/// @param event [IN] 日志事件智能指针
		/// @return 过滤决策
		FilterDecision Decide(const LogEventPtr &event) override;

		/// @brief 获取过滤器名称
		/// @return 过滤器名称
		std::string GetName() const override { return "AndFilter"; }

		/// @brief 克隆过滤器
		/// @return 过滤器智能指针
		Pointer Clone() const override;
	};

	/// @brief 或过滤器（任一子过滤器接受即接受）
	class IDLOG_API OrFilter : public CompositeFilter
	{
	public:
		/// @brief 过滤日志事件
		/// @param event [IN] 日志事件智能指针
		/// @return 过滤决策
		FilterDecision Decide(const LogEventPtr &event) override;

		/// @brief 获取过滤器名称
		/// @return 过滤器名称
		std::string GetName() const override { return "OrFilter"; }

		/// @brief 克隆过滤器
		/// @return 过滤器智能指针
		Pointer Clone() const override;
	};

	/// @brief 非过滤器（反转决策）
	class IDLOG_API NotFilter : public Filter
	{
	public:
		/// @brief 构造函数
		/// @param filter [IN] 需要取反的子过滤器
		explicit NotFilter(const Pointer &filter);
		/// @brief 析构函数
		virtual ~NotFilter();

		/// @brief 过滤日志事件
		/// @param event [IN] 日志事件智能指针
		/// @return 过滤决策
		FilterDecision Decide(const LogEventPtr &event) override;

		/// @brief 获取过滤器名称
		/// @return 过滤器名称
		std::string GetName() const override { return "NotFilter"; }

		/// @brief 克隆过滤器
		/// @return 过滤器智能指针
		Pointer Clone() const override;

		/// @brief 设置子过滤器
		/// @param filter [IN] 需要取反的子过滤器
		void SetFilter(const Pointer &filter);

		/// @brief 获取子过滤器
		/// @return 需要取反的子过滤器
		Pointer GetFilter() const;

	private:
		/// @brief 非过滤器实现结构体前向声明
		struct Impl;
	private:
		Impl* m_pImpl; ///< 非过滤器实现指针
	};

} // namespace IDLog

#endif // !IDLOG_FILTER_FILTER_H
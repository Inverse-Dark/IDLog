/**
 * @Description: 过滤器基类源文件
 * @Author: InverseDark
 * @Date: 2025-12-18 20:36:52
 * @LastEditTime: 2025-12-18 20:46:58
 * @LastEditors: InverseDark
 */
#include "IDLog/Filter/Filter.h"

namespace IDLog
{

	void Filter::AddFilter(const Pointer &filter)
	{
		// 基类实现为空，由子类重写
	}

	std::vector<Filter::Pointer> Filter::GetFilters() const
	{
		return {};
	}

	void Filter::ClearFilters()
	{
		// 基类实现为空，由子类重写
	}

	/// @brief 组合过滤器实现结构体
	struct CompositeFilter::Impl
	{
		std::vector<Pointer> filters; ///< 子过滤器列表
	};

	CompositeFilter::CompositeFilter()
		: m_pImpl(new Impl)
	{
	}

	CompositeFilter::~CompositeFilter()
	{
		delete m_pImpl;
	}

	FilterDecision CompositeFilter::Decide(const LogEventPtr &event)
	{
		// 默认实现：如果没有子过滤器，返回中立
		if (m_pImpl->filters.empty())
		{
			return FilterDecision::NEUTRAL;
		}

		// 默认与逻辑
		return AndFilter().Decide(event);
	}

	void CompositeFilter::AddFilter(const Pointer &filter)
	{
		if (filter)
		{
			m_pImpl->filters.push_back(filter);
		}
	}

	std::vector<Filter::Pointer> CompositeFilter::GetFilters() const
	{
		return m_pImpl->filters;
	}

	void CompositeFilter::ClearFilters()
	{
		m_pImpl->filters.clear();
	}

	FilterDecision AndFilter::Decide(const LogEventPtr &event)
	{
		// 如果没有子过滤器，返回中立
		if (m_pImpl->filters.empty())
		{
			return FilterDecision::NEUTRAL;
		}

		// 默认接受
		FilterDecision result = FilterDecision::ACCEPT;

		// 遍历子过滤器
		for (const auto &filter : m_pImpl->filters)
		{
			// 获取子过滤器的决策
			FilterDecision decision = filter->Decide(event);

			// 如果有一个拒绝，则整体拒绝
			if (decision == FilterDecision::DENY)
			{
				return FilterDecision::DENY;
			}
			// 如果有一个中立，则结果为中立
			else if (decision == FilterDecision::NEUTRAL)
			{
				result = FilterDecision::NEUTRAL;
			}
		}
		// 返回最终决策
		return result;
	}

	Filter::Pointer AndFilter::Clone() const
	{
		auto cloned = std::make_shared<AndFilter>();
		for (const auto &filter : m_pImpl->filters)
		{
			cloned->AddFilter(filter->Clone());
		}
		return cloned;
	}

	FilterDecision OrFilter::Decide(const LogEventPtr &event)
	{
		// 如果没有子过滤器，返回中立
		if (m_pImpl->filters.empty())
		{
			return FilterDecision::NEUTRAL;
		}

		// 默认拒绝
		FilterDecision result = FilterDecision::DENY;

		// 遍历子过滤器
		for (const auto &filter : m_pImpl->filters)
		{
			// 获取子过滤器的决策
			FilterDecision decision = filter->Decide(event);

			// 如果有一个接受，则整体接受
			if (decision == FilterDecision::ACCEPT)
			{
				return FilterDecision::ACCEPT;
			}
			// 如果有一个中立，则结果为中立
			else if (decision == FilterDecision::NEUTRAL)
			{
				result = FilterDecision::NEUTRAL;
			}
		}
		// 返回最终决策
		return result;
	}

	Filter::Pointer OrFilter::Clone() const
	{
		auto cloned = std::make_shared<OrFilter>();
		for (const auto &filter : m_pImpl->filters)
		{
			cloned->AddFilter(filter->Clone());
		}
		return cloned;
	}

	/// @brief 非过滤器实现结构体
	struct NotFilter::Impl
	{
		Pointer filter; ///< 需要取反的子过滤器
	};

	NotFilter::NotFilter(const Pointer &filter)
		: m_pImpl(new Impl)
	{
		m_pImpl->filter = filter;
	}

	NotFilter::~NotFilter()
	{
		delete m_pImpl;
	}

	FilterDecision NotFilter::Decide(const LogEventPtr &event)
	{
		// 如果没有子过滤器，返回中立
		if (!m_pImpl->filter)
		{
			return FilterDecision::NEUTRAL;
		}

		// 获取子过滤器的决策
		FilterDecision decision = m_pImpl->filter->Decide(event);

		// 反转决策
		switch (decision)
		{
		case FilterDecision::ACCEPT:
			return FilterDecision::DENY;
		case FilterDecision::DENY:
			return FilterDecision::ACCEPT;
		case FilterDecision::NEUTRAL:
			return FilterDecision::NEUTRAL;
		default:
			return FilterDecision::NEUTRAL;
		}
	}

	Filter::Pointer NotFilter::Clone() const
	{
		return std::make_shared<NotFilter>(m_pImpl->filter ? m_pImpl->filter->Clone() : nullptr);
	}

	void NotFilter::SetFilter(const Pointer &filter)
	{
		m_pImpl->filter = filter;
	}

	Filter::Pointer NotFilter::GetFilter() const
	{
		return m_pImpl->filter;
	}

} // namespace IDLog
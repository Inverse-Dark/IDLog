/** 
 * @Description: 配置头文件
 * @Author: InverseDark
 * @Date: 2025-12-21 11:40:26
 * @LastEditTime: 2025-12-21 15:14:17
 * @LastEditors: InverseDark
 */
#ifndef IDLOG_CORE_CONFIGURATION_H
#define IDLOG_CORE_CONFIGURATION_H

#include "IDLog/Core/LogFactory.h"

#include <map>
#include <functional>

namespace IDLog
{
	/// @brief 配置类
	/// @details 提供全局配置管理接口
	class IDLOG_API Configuration
	{
	public:
		/// @brief 配置选项结构体
		struct Options
		{
			/// @brief 全局配置选项结构体
			struct Global
			{
				LogLevel rootLevel = LogLevel::INFO; ///< 根日志级别，默认INFO
				bool enableStatistics = false;	 ///< 是否启用日志统计，默认禁用
				uint64_t statisticsInterval = 60; ///< 日志统计间隔，单位秒，默认60秒
			} global;								///< 全局配置选项
			
			/// @brief 日志器配置选项结构体
			struct LoggerConfig
			{
				LogLevel level = LogLevel::INFO;   ///< 日志级别，默认INFO
				std::vector<std::string> appenders; ///< 输出器名称列表
				std::vector<std::string> filters;	 ///< 过滤器名称列表
				bool additive = true;			   ///< 是否继承父日志器的输出器，默认继承
			};

			/// @brief 过滤器配置选项结构体
			struct FilterConfig
			{
				std::string type;				   ///< 过滤器类型
				std::map<std::string, std::string> params; ///< 过滤器参数键值对
			};

			/// @brief 输出器配置选项结构体
			struct AppenderConfig
			{
				std::string type;				   ///< 输出器类型
				std::string formatter;			   ///< 格式化器名称
				std::map<std::string, std::string> params; ///< 输出器参数键值对
			};

			/// @brief 格式化器配置选项结构体
			struct FormatterConfig
			{
				std::string type;				   ///< 格式化器类型
				std::map<std::string, std::string> params; ///< 格式化器参数键值对
			};
			
			std::map<std::string, LoggerConfig> loggers; ///< 日志器配置选项映射，键为日志器名称
			std::map<std::string, FilterConfig> filters; ///< 过滤器配置选项映射，键为过滤器名称
			std::map<std::string, AppenderConfig> appenders; ///< 输出器配置选项映射，键为输出器名称
			std::map<std::string, FormatterConfig> formatters; ///< 格式化器配置选项映射，键为格式化器名称

			/// @brief 验证配置选项的有效性
			/// @return 如果配置有效，返回true；否则返回false
			bool Validate() const;

			/// @brief 清空配置选项
			void Clear();
		};

	public:
		/// @brief 获取全局配置实例
		/// @return 全局配置实例引用
		static Configuration &GetInstance();

		/// @brief 拷贝构造函数(禁用)
		/// @param [IN] 另一个Configuration对象
		Configuration(const Configuration &) = delete;
		/// @brief 拷贝赋值运算符(禁用)
		/// @param [IN] 另一个Configuration对象
		/// @return 当前Configuration对象的引用
		Configuration &operator=(const Configuration &) = delete;

		/// @brief 加载配置文件
		/// @param filename [IN] 配置文件名
		/// @return 加载成功返回true，否则返回false
		bool LoadFromFile(const std::string &filename);

		/// @brief 加载配置内容字符串
		/// @param content [IN] 配置内容字符串
		/// @return 加载成功返回true，否则返回false
		bool LoadFromString(const std::string &content);

		/// @brief 保存配置到文件
		/// @param filename [IN] 目标文件名
		/// @return 保存成功返回true，否则返回false
		bool SaveToFile(const std::string &filename);

		/// @brief 重新加载配置
		/// @return 重新加载成功返回true，否则返回false
		bool Reload();

		/// @brief 获取当前配置选项
		/// @return 配置选项引用
		const Options& GetOptions() const;

		/// @brief 应用配置选项
		/// @param options [IN] 配置选项
		/// @return 应用成功返回true，否则返回false
		bool ApplyOptions(const Options &options);

		/// @brief 设置Log工厂实例
		/// @param factory [IN] Log工厂实例智能指针
		void SetFactory(const LogFactory::Pointer &factory);

		/// @brief 注册配置变更回调函数
		/// @param callback [IN] 回调函数
		void RegisterChangeCallback(const std::function<void()> &callback);

		/// @brief 根据配置创建过滤器实例
		/// @param config [IN] 过滤器配置选项
		/// @return 过滤器智能指针
		Filter::Pointer CreateFilterFromConfig(const Options::FilterConfig &config);

		/// @brief 根据配置创建输出器实例
		/// @param config [IN] 输出器配置选项
		/// @return 输出器智能指针
		LogAppender::Pointer CreateAppenderFromConfig(const Options::AppenderConfig &config);

		/// @brief 根据配置创建格式化器实例
		/// @param config [IN] 格式化器配置选项
		/// @return 格式化器智能指针
		Formatter::Pointer CreateFormatterFromConfig(const Options::FormatterConfig &config);

		/// @brief 启用或禁用自动重新加载配置
		/// @param enable [IN] 是否启用自动重新加载
		/// @param intervalSeconds [IN] 重新加载间隔时间，单位秒，默认60秒
		void EnableAutoReload(bool enable, uint64_t intervalSeconds = 60);

		/// @brief 检查是否启用自动重新加载配置
		/// @return 启用返回true，否则返回false
		bool IsAutoReloadEnabled() const;

		/// @brief 检查配置是否已应用
		/// @return 配置已应用返回true，否则返回false
		bool IsApplied() const;
	private:
		/// @brief 构造函数
		Configuration();

		/// @brief 析构函数
		~Configuration();

		/// @brief 自动重新加载线程函数
		void AutoReloadThread();

		/// @brief 更新配置解析器内容
		/// @return 更新成功返回true，否则返回false
		void UpdateParserFromOptions();

		/// @brief 解析配置
		/// @return 解析成功返回true，否则返回false
		bool UpdateOptionsFromParser();

		/// @brief 应用配置选项
		/// @return 应用成功返回true，否则返回false
		bool ApplyOptions();

	private:
		/// @brief 配置实现结构体前向声明
		struct Impl;
	private:
		Impl* m_pImpl; ///< 配置实现指针
	};

} // namespace IDLog


#endif // !IDLOG_CORE_CONFIGURATION_H
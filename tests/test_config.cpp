/**
 * @Description: 配置加载测试
 * @Author: InverseDark
 * @Date: 2025-12-21 14:41:06
 * @LastEditTime: 2025-12-21 14:46:35
 * @LastEditors: InverseDark
 */
#include "IDLog/IDLog.h"
#include <iostream>
#include <fstream>
#include <cassert>

void CreateTestConfig(const std::string& filename)
{
    std::ofstream file(filename);
    file << "[global]\nrootLevel=ERROR\n";
    file.close();
}

void TestConfigLoad()
{
    std::cout << "[Test] Configuration Load..." << std::endl;
    std::string filename = "test_config.ini";
    CreateTestConfig(filename);
    
    auto& config = IDLog::Configuration::GetInstance();
    bool ret = config.LoadFromFile(filename);
    assert(ret == true);
    
    auto root = IDLog::LoggerManager::GetInstance().GetRootLogger();
    assert(root->GetLevel() == IDLog::LogLevel::ERR);
    
    std::cout << "  -> Passed" << std::endl;
    std::filesystem::remove(filename);
}

int main()
{
    std::cout << "=== IDLog Config Tests ===" << std::endl;
    TestConfigLoad();
    std::cout << "=== All Config Tests Passed ===" << std::endl;
    return 0;
}
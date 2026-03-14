# Parasolid 几何修复工具

基于 **Siemens Parasolid** 内核的专业CAD几何修复工具软件。

## 功能特性

### 五大核心功能

1. **缝合 (Sewing)**
   - 将多个独立的面缝合为连续的壳体或实体
   - 支持多公差缝合
   - 智能缝合分析

2. **检查修复 (Check & Repair)**
   - 9种几何问题检测：
     - 狭小面 (Narrow Face)
     - 微小面 (Small Face)
     - 尖刺 (Spike)
     - 重复面 (Duplicate Face)
     - 相交面 (Intersecting Face)
     - 冗余点 (Redundant Vertex)
     - 冗余边 (Redundant Edge)
     - 微小边 (Small Edge)
     - 间隙 (Gap)
   - 支持筛选和批量修复
   - 逐一修复或一键修复

3. **N边曲面补面 (Patching)**
   - 自动填充N边形孔洞
   - 多种曲面构造方法：
     - GeomPlate 曲面拟合
     - Coons 曲面
     - B-Spline 曲面
     - 直纹面
   - 支持切向连续

4. **删除面 (Face Removal)**
   - 智能删除选定面
   - 自动填补产生的孔洞
   - 多种填补策略

5. **抽梁 (Beam Extraction)**
   - 从实体或壳体提取梁结构
   - 自动识别截面类型
   - 骨架提取

## 技术架构

```
ParasolidGeometryRepair/
├── include/
│   ├── core/          # 核心引擎
│   │   ├── PsKernel.h
│   │   ├── SewingEngine.h
│   │   ├── CheckRepairEngine.h
│   │   ├── PatchingEngine.h
│   │   ├── FaceRemoveEngine.h
│   │   └── BeamExtractEngine.h
│   ├── geometry/      # 几何封装
│   │   ├── PsTypes.h
│   │   ├── PsBody.h
│   │   ├── PsFace.h
│   │   ├── PsEdge.h
│   │   └── PsVertex.h
│   ├── io/            # 文件IO
│   │   ├── PsImporter.h
│   │   ├── PsExporter.h
│   │   ├── PsSTEPImporter.h
│   │   └── PsXTImporter.h
│   └── ui/            # UI层
│       ├── MainWindow.h
│       ├── CheckResultDialog.h
│       └── ...
└── src/               # 实现文件
```

## 构建要求

### 依赖
- **CMake** 3.20+
- **Qt 6.x** (Core, Gui, Widgets, OpenGLWidgets, 3DCore, 3DRender, 3DInput, 3DExtras)
- **Parasolid SDK** (需商业许可)
- **C++17** 编译器

### 构建步骤

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Parasolid配置

设置环境变量或在CMake中指定：
```bash
export PARASOLID_ROOT=/path/to/parasolid
```

## 使用说明

### 基本流程

1. **导入模型**
   - 支持 STEP, IGES, Parasolid XT 格式

2. **执行检查修复**
   - 点击菜单 "几何 -> 检查修复"
   - 设置检查选项
   - 执行检查
   - 筛选结果
   - 逐一修复或一键修复

3. **其他功能**
   - 缝合：合并多个面
   - 补面：填充孔洞
   - 删除面：移除指定面并填补
   - 抽梁：提取梁结构

## 许可证

本项目的代码采用 MIT 许可证。

**注意**: 使用本软件需要有效的 **Siemens Parasolid 许可证**。

## 版本历史

### v1.0.0
- 初始版本
- 实现五大核心功能
- 完整的UI界面
- 支持 STEP/Parasolid 导入导出

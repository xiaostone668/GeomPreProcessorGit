# STEP测试文件说明

本目录包含用于测试CAD几何修复功能的各种有缺陷的STEP文件。

## 文件列表 (共9个)

| # | 文件名 | 大小 | 错误类型 | 描述 |
|---|--------|------|----------|------|
| 1 | `01_gap_between_faces.step` | ~5KB | **缝隙** | 顶部面偏移0.2mm产生间隙 |
| 2 | `02_overlapping_faces.step` | ~6KB | **重叠面** | 底面有重叠的内环 |
| 3 | `03_open_shell_missing_face.step` | ~4.6KB | **开放壳体** | 缺少顶部面 |
| 4 | `04_tiny_face.step` | ~7KB | **微小面** | 4个极小的三角形面（0.002mm级别） |
| 5 | `05_inverted_normals.step` | ~5KB | **法向不一致** | 多个面的法向方向反转 |
| 6 | `06_degenerate_edge.step` | ~5.7KB | **退化边** | 零长度边（起点终点重合） |
| 7 | `07_self_intersection.step` | ~6KB | **自相交** | 内部自相交的几何元素 |
| 8 | `08_non_manifold_vertex.step` | ~5KB | **非流形顶点** | 顶点连接多个壳体 |
| 9 | `09_short_edge.step` | ~5.5KB | **短边** | 包含极短的边（0.001mm） |

## 使用说明

这些文件可以直接用以下CAD软件打开：
- **FreeCAD** (免费开源) - 推荐
- **SolidWorks**
- **CATIA**
- **NX**
- **AutoCAD**
- **Fusion 360**

## 预期测试结果

| 文件 | 修复软件应检测到的错误 |
|------|----------------------|
| 01_gap_between_faces.step | 间隙检测、自由边、未闭合壳体 |
| 02_overlapping_faces.step | 面重叠、多余几何、重复面 |
| 03_open_shell_missing_face.step | 开放边界、缺失面、未闭合实体 |
| 04_tiny_face.step | 微小几何、退化面、短边 |
| 05_inverted_normals.step | 法向错误、反向面、方向不一致 |
| 06_degenerate_edge.step | 退化边、零长度边、重合顶点 |
| 07_self_intersection.step | 自相交、面交叉、无效拓扑 |
| 08_non_manifold_vertex.step | 非流形拓扑、顶点共享错误 |
| 09_short_edge.step | 短边、微小边、边长小于容差 |

## 技术规格

- **格式**: STEP AP214 (ISO 10303-21)
- **单位**: 毫米 (mm)
- **坐标系**: 右手坐标系
- **精度**: 1.0E-06 mm

## 下载到Windows

### 方法1: 使用SCP (PowerShell/Git Bash)
```bash
# 在Windows上打开PowerShell，执行：
scp username@服务器IP:/home/gem/test_step_files/*.step C:\STEP_Test_Files\
```

### 方法2: 使用WinSCP (图形界面)
1. 下载安装 [WinSCP](https://winscp.net/)
2. 输入服务器IP、用户名、密码连接
3. 导航到 `/home/gem/test_step_files/`
4. 拖拽文件到本地目录

### 方法3: 打包下载（推荐）
```bash
# 在服务器上打包
cd /home/gem && tar -czvf test_step_files.tar.gz test_step_files/

# 然后在Windows下载
scp username@服务器IP:/home/gem/test_step_files.tar.gz C:\Downloads\
cd C:\Downloads
tar -xzf test_step_files.tar.gz
```

## 测试建议

1. **逐一测试**: 建议先单独测试每个文件，确认修复软件能正确识别错误类型
2. **对比修复**: 记录修复前后的几何状态，验证修复效果
3. **批量测试**: 使用脚本批量导入所有文件，测试软件的稳定性

## 常见问题

**Q: 文件打开后显示不完整？**
A: 这是正常的，因为这些文件故意包含错误。CAD软件可能无法正确显示有缺陷的几何。

**Q: 修复软件报告不同错误？**
A: 不同的几何修复引擎可能识别出不同类型的错误，这取决于算法实现。

**Q: 可以用这些文件测试其他功能吗？**
A: 可以，这些文件也适用于测试：
- 几何检查/验证功能
- 文件导入/导出功能
- 错误报告功能
- 自动修复功能

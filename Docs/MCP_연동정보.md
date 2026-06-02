# Unreal MCP 연동 정보

> Epic 공식 "Unreal MCP" 플러그인 (UE 5.8 내장, Experimental)  
> Claude Code ↔ 언리얼 에디터 연결 설정 기록  
> 최초 설정일: 2026-06-01

---

## 1. 핵심 요약

| 항목 | 값 |
|---|---|
| 플러그인 이름 (에디터 표시) | **Unreal MCP** |
| 실제 폴더명 | `ModelContextProtocol` |
| 서버 주소 | `http://127.0.0.1:8000/mcp` |
| 전송 방식 | HTTP |
| MCP 서버 이름 (Claude 등록명) | `unreal-mcp` |
| 기본 포트 | `8000` |
| 기본 URL Path | `/mcp` |

---

## 2. 플러그인 경로 (엔진 내장)

```
C:\Program Files\Epic Games\UE_5.8\Engine\Plugins\Experimental\ModelContextProtocol\
```

함께 쓰는 보조 플러그인:
```
...\Engine\Plugins\Experimental\Toolsets\MCPClientToolset\
```

설정 파일 소스 (참고용):
```
...\ModelContextProtocol\Source\ModelContextProtocolEngine\Private\ModelContextProtocolClientConfig.cpp
...\ModelContextProtocol\Source\ModelContextProtocolEngine\Private\ModelContextProtocolSettings.cpp
```

---

## 3. 활성화 / 시작 방법

1. **플러그인 활성화**: 에디터 → 플러그인 → "Unreal MCP" + "MCP Client Toolset" 체크
2. **서버 자동 시작**: 프로젝트 세팅 → "MCP" 검색 → Server 카테고리 → **Auto Start Server** ☑
   - 또는 에디터 실행 시 커맨드라인 `-StartModelContextProtocolServer`
   - 포트 변경: `-ModelContextProtocolPort=XXXX`
3. **에디터 재시작** (활성화/설정 적용)

> ⚠️ **에디터가 켜져 있어야 MCP 서버가 살아있다.** 에디터를 닫으면 연결 끊김 → 다시 켜면 Auto Start로 복구.

---

## 4. Claude Code 등록

### 자동: 프로젝트 루트의 `.mcp.json`
```json
{
	"mcpServers": {
		"unreal-mcp": {
			"type": "http",
			"url": "http://127.0.0.1:8000/mcp"
		}
	}
}
```

### 수동(CLI):
```bash
claude mcp add --transport http unreal-mcp http://127.0.0.1:8000/mcp
```

### 연결 확인:
```bash
claude mcp list      # unreal-mcp: ... - ✓ Connected
```

> 새 MCP 서버 도구는 **Claude Code 재시작 후** 세션에 로드됨.

---

## 5. 서버 동작 확인 (디버그)

```bash
# 포트 LISTENING 확인
netstat -ano | grep ":8000"

# MCP initialize 핑 (HTTP 200 이면 정상)
curl -s -o /dev/null -w "%{http_code}\n" -X POST http://127.0.0.1:8000/mcp \
  -H "Content-Type: application/json" \
  -H "Accept: application/json, text/event-stream" \
  -d '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05","capabilities":{},"clientInfo":{"name":"probe","version":"0.0.1"}}}'
```

---

## 6. Toolset 사용 방식

Epic MCP는 **toolset 단위로 도구를 로드**한다. (한 번에 다 안 올라옴)

- `list_toolsets` — 사용 가능한 toolset 목록
- `load_toolset <name>` — 해당 toolset의 도구 등록 (**다음 턴부터 사용 가능**)
- `describe_toolset <name>` — toolset 내 도구 상세/스키마

### 주요 Toolset → 우리 작업 매핑

| Toolset | 용도 |
|---|---|
| `...core.scene.SceneTools` | 레벨/액터 배치, 카메라, 아웃라이너 |
| `...core.actor.ActorTools` | 액터 트랜스폼/컴포넌트/계층 |
| `...core.blueprint.BlueprintTools` | 각성 카드, 보스 BP 생성/편집 |
| `...core.data_asset.DataAssetTools` | BossDataAsset, 각성 카드 데이터 |
| `...core.data_table.DataTableTools` | 밸런스 수치 테이블 |
| `...core.curve_table.CurveTableTools` | 성장 곡선 |
| `...core.material.MaterialTools` | 약점 하이라이트, 장판 데칼 |
| `...core.texture.TextureTools` | 텍스처 |
| `...core.static_mesh.StaticMeshTools` | 정적 메시 |
| `...core.object.ObjectTools` | 오브젝트/클래스 프로퍼티, 클래스 탐색 |
| `...core.primitive.PrimitiveTools` | 프리미티브 지오메트리 |
| `...core.programmatic.ProgrammaticToolset` | Python으로 여러 도구 일괄 실행 |
| `ToolsetRegistry.EditorAppToolset` | 콘솔 변수, 선택, 뷰포트 카메라 |
| `ToolsetRegistry.LogsToolset` | 출력 로그 읽기, 로그 verbosity 제어 |
| `ToolsetRegistry.AgentSkillToolset` | 스킬 목록/읽기/생성 |

---

## 7. 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| `claude mcp list`에서 연결 안 됨 | 에디터 꺼짐 or 서버 미시작 → 에디터 켜고 Auto Start 확인 |
| 도구가 세션에 안 보임 | Claude Code 재시작 |
| load_toolset 후 도구 호출 실패 | 같은 턴에는 안 됨 → 다음 턴에 호출 |
| 포트 충돌 | `-ModelContextProtocolPort=` 로 변경 후 `.mcp.json` URL도 같이 수정 |
| **`Invalid session id` (-32600)** | **에디터 재시작 시 서버 세션이 새로 발급됨. Claude Code가 옛 세션 ID를 캐싱 → `/mcp`에서 unreal-mcp Reconnect, 또는 Claude Code 재시작** |

> ⚠️ **반복 패턴 주의**: C++ 새 클래스 빌드 → 에디터 닫고 빌드 → 에디터 재실행. 이때마다 MCP 서버 세션이 갱신되므로 **에디터 재실행 후 `/mcp` 재연결**이 필요하다. (또는 빌드 후 Claude Code 재시작.)

> ✅ **시작 순서 (중요)**: 반드시 **① 에디터 먼저 켜서 MCP 서버 ready 확인 → ② 그다음 Claude Code 시작**. 순서가 반대면 Claude Code 세션 도구 목록에 unreal-mcp 도구가 등록되지 않는다. 이 경우 `/mcp` Reconnect로도 현재 세션 도구 목록은 갱신되지 않으므로 **Claude Code를 다시 시작**해야 한다. (서버 정상 확인: `curl ... :8000/mcp` → 200)

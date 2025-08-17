# UE5 Multiplayer Shooter
UE5 C++로 경쟁형 멀티플레이어 슈터를 구현하고 멀티 기능을 플러그인으로 모듈화해 재사용 가능하게 만드는 것이 목적.

---

<br>

## 강의 목표 및 특징

| 구분 | 내용 |
|---|---|
| 핵심 목표 | 세션, 트래블, 복제, 지연 보상, HUD, 매치 흐름을 갖춘 멀티 슈터 완성. |
| 학습 범위 | Online Session(Create/Find/Join), `ServerTravel`/`ClientTravel`, Replication·RPC, 커스텀 MatchState, Warmup/Cooldown, Elim 알림, HUD(HP·Shield·Ammo·Grenade), 무기(AR/SMG/권총/로켓/그레네이드 런처/스나이퍼/투척). |
| 네트워킹 원칙 | 서버 권한 모델, 신뢰 경계 명확화, 서버 측 검증으로 치트 저항 강화. |
| 플러그인 전략 | 멀티 기능을 플러그인으로 캡슐화하고 설정만으로 타 프로젝트 적용. |
| 성능·지연 보상 | Client-side prediction, Server-side rewind, Relevancy·UpdateRate 튜닝, 트래픽·틱 비용 절감, Network Profiler·Net Emulation 활용. |
| C++↔Blueprint 운영 | 네트워킹·무기 코어 C++, UI·애님·튜닝 BP. 권장 비중 7:3. |

<br>

## 커리큘럼

- 프로젝트 셋업: 전용 서버, OnlineSubsystem, 플러그인 골격
- 세션 파이프라인: Create/Find/Join, 로비→게임 매치메이킹
- 맵 트래블: `ServerTravel`·`ClientTravel`, 상태 동기화, 실패 복구
- 복제 기초: 변수·함수·멀티캐스트, Owner·Relevancy·ReplicationCondition
- 입력·이동 네트워크화: CharacterMovement 복제, 스무딩
- 무기 프레임워크: 공통 베이스, 탄약·재장전, 발사 모드, 투사체·폭발
- HUD·게임 루프: MatchState 커스터마이즈, Warmup/Cooldown, Elim/KDA UI
- 지연 보상: 클라이언트 예측, 서버 리와인드, 히트스캔·투사체 판정
- 보안·권한: RPC 인자 검증, 서버 계산 집중, 클라 신뢰 최소화
- 성능 최적화: 복제 빈도·조건, Net Cull, UpdateRate, 패킷 예산
- 플러그인화: 설정 노출, 의존성 정리, 타 프로젝트 통합
- 테스트·배포: Net Emulation, 다중 클라 검증, 전용 서버 스케일링

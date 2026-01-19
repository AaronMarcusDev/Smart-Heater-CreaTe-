class TargetMapper {
public:
  TargetMapper(int minAngle, int maxAngle)
    : minAngle(minAngle), maxAngle(maxAngle) {}

  int gridXToServoAngle(int x) {
    // Map 0–31 → servo angle range
    return map(x, 0, 31, minAngle, maxAngle);
  }

private:
  int minAngle;
  int maxAngle;
};
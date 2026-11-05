// Regression test: objects (global variables) of typical blades and versors in
// every model, so that the harness dumps their multivector coordinates and
// interpretation (mvInt). The book figures hardly use c5ga and i2ga.

function algebra_objects()
{
    default_model(e3ga);
    ::e_scalar = 2.5;
    ::e_vector = e1 + 2 e2 - 0.5 e3;
    ::e_bivector = (e1 + 2 e2) ^ (0.3 e1 - e2 + 1.5 e3);
    ::e_trivector = 0.7 e1^e2^e3;
    ::e_rotor = exp(-0.3 e1^e2) exp(0.2 e2^e3);
    ::e_odd_versor = (e1 + 0.2 e3) (e2 - 0.4 e1);

    default_model(p3ga);
    ::p_point = e0 + e1 + 2 e2 - 0.5 e3;
    ::p_direction = 0.3 e1 - e2 + 1.5 e3;
    ::p_line = (e0 + e1 + 2 e2) ^ (0.3 e1 - e2 + 1.5 e3);
    ::p_plane = (e0 + e1) ^ (e0 + e2) ^ (e0 + 0.5 e3);
    ::p_rotor = exp(-0.3 e1^e2);

    default_model(c3ga);
    ::c_point = no + e1 + 2 e2 - 0.5 e3 + 2.625 ni;
    ::c_point_pair = (no + e1 + 0.5 ni) ^ (no - e2 + 0.5 ni);
    ::c_circle = (no + e1 + 0.5 ni) ^ (no - e2 + 0.5 ni) ^ (no + e3 + 0.5 ni);
    ::c_sphere = (no + e1 + 0.5 ni) ^ (no - e2 + 0.5 ni) ^ (no + e3 + 0.5 ni) ^ (no + 0.5 ni);
    ::c_dual_sphere = no + e1 + (0.5 - 2) ni;
    ::c_line = (no + e1 + 0.5 ni) ^ (e2 + e3) ^ ni;
    ::c_plane = (no + e1 + 0.5 ni) ^ (e2 + e3) ^ e1 ^ ni;
    ::c_flat_point = (no + e1 + 0.5 ni) ^ ni;
    ::c_tangent = (no + e1 + 0.5 ni) ^ (e2 + ((no + e1 + 0.5 ni) . e2) ni);
    ::c_free_vector = e2 ^ ni;
    ::c_free_bivector = e1 ^ e2 ^ ni;
    ::c_translator = exp(-0.5 (e1 + e3)^ni);
    ::c_motor = exp(-0.3 e1^e2) exp(-0.5 (e1 + e3)^ni);

    default_model(c5ga);
    ::c5_point = no + e1 + 2 e2 - 0.5 e3 + 0.25 e4 - e5 + 2.78125 ni;
    ::c5_point_pair = (no + e1 + 0.5 ni) ^ (no - e4 + 0.5 ni);
    ::c5_circle = (no + e1 + 0.5 ni) ^ (no - e2 + 0.5 ni) ^ (no + e3 + 0.5 ni);
    ::c5_line = (no + e1 + 0.5 ni) ^ (e2 + e5) ^ ni;
    ::c5_rotor = exp(-0.3 e1^e4);

    default_model(i2ga);
    ::i2_point = no + e1 + 2 e2 + 2.5 ni;
    ::i2_point2 = no + e1 + 2 e2 + 0.5 go + 2.5 ni - gi;
    ::i2_line = (no + e1 + 0.5 ni) ^ e2 ^ ni;
    ::i2_circle = (no + e1 + 0.5 ni) ^ (no - e2 + 0.5 ni) ^ (no + 0.5 ni);
    ::i2_rotor = exp(-0.3 e1^e2);
}

load("//bazel:tachyon_cc.bzl", "tachyon_cc_library")

def _generate_ec_point_impl(ctx):
    arguments = [
        "--out=%s" % (ctx.outputs.out.path),
        "--namespace=%s" % (ctx.attr.namespace),
        "--base_field=%s" % (ctx.attr.base_field),
        "--base_field_hdr=%s" % (ctx.attr.base_field_hdr),
        "--scalar_field=%s" % (ctx.attr.scalar_field),
        "--scalar_field_hdr=%s" % (ctx.attr.scalar_field_hdr),
    ]
    if len(ctx.attr.class_name) > 0:
        arguments.append("--class=%s" % (ctx.attr.class_name))

    for a in ctx.attr.a:
        arguments.append("-a=%s" % (a))

    for d in ctx.attr.d:
        arguments.append("-d=%s" % (d))

    for x in ctx.attr.x:
        arguments.append("-x=%s" % (x))

    for y in ctx.attr.y:
        arguments.append("-y=%s" % (y))

    if len(ctx.attr.mul_by_a_override) > 0:
        arguments.append("--mul_by_a_override=%s" % (ctx.attr.mul_by_a_override))

    if len(ctx.attr.glv_coeffs) > 0:
        for endomorphism_coefficient in ctx.attr.endomorphism_coefficient:
            arguments.append("--endomorphism_coefficient=%s" % (endomorphism_coefficient))

        arguments.append("--lambda=%s" % (ctx.attr.lambda_))

        for glv_coeff in ctx.attr.glv_coeffs:
            arguments.append("--glv_coefficients=%s" % glv_coeff)

    ctx.actions.run(
        tools = [ctx.executable._tool],
        executable = ctx.executable._tool,
        outputs = [ctx.outputs.out],
        arguments = arguments,
    )

    return [DefaultInfo(files = depset([ctx.outputs.out]))]

generate_ec_point = rule(
    implementation = _generate_ec_point_impl,
    attrs = {
        "out": attr.output(mandatory = True),
        "namespace": attr.string(mandatory = True),
        "class_name": attr.string(),
        "base_field": attr.string(mandatory = True),
        "base_field_hdr": attr.string(mandatory = True),
        "scalar_field": attr.string(mandatory = True),
        "scalar_field_hdr": attr.string(mandatory = True),
        "a": attr.string_list(mandatory = True),
        "d": attr.string_list(mandatory = True),
        "x": attr.string_list(mandatory = True),
        "y": attr.string_list(mandatory = True),
        "mul_by_a_override": attr.string(),
        "endomorphism_coefficient": attr.string_list(),
        "lambda_": attr.string(),
        "glv_coeffs": attr.string_list(),
        "_tool": attr.label(
            # TODO(chokobole): Change it to "exec" we can build it on macos.
            cfg = "target",
            executable = True,
            allow_single_file = True,
            default = Label("@kroma_network_tachyon//tachyon/math/elliptic_curves/twisted_edwards/generator"),
        ),
    },
)

def generate_ec_points(
        name,
        namespace,
        base_field,
        base_field_hdr,
        base_field_dep,
        scalar_field,
        scalar_field_hdr,
        scalar_field_dep,
        a,
        d,
        x,
        y,
        class_name = "",
        mul_by_a_override = "",
        endomorphism_coefficient = [],
        lambda_ = "",
        glv_coeffs = [],
        **kwargs):
    for n in [
        ("{}_gen_hdr".format(name), "{}.h".format(name)),
        ("{}_gen_gpu_hdr".format(name), "{}_gpu.h".format(name)),
    ]:
        generate_ec_point(
            namespace = namespace,
            class_name = class_name,
            base_field = base_field,
            base_field_hdr = base_field_hdr,
            scalar_field = scalar_field,
            scalar_field_hdr = scalar_field_hdr,
            a = a,
            d = d,
            x = x,
            y = y,
            mul_by_a_override = mul_by_a_override,
            endomorphism_coefficient = endomorphism_coefficient,
            lambda_ = lambda_,
            glv_coeffs = glv_coeffs,
            name = n[0],
            out = n[1],
        )

    tachyon_cc_library(
        name = name,
        hdrs = [":{}_gen_hdr".format(name)],
        deps = [
            base_field_dep,
            scalar_field_dep,
            "//tachyon/math/elliptic_curves/twisted_edwards:points",
            "//tachyon/math/elliptic_curves/twisted_edwards:te_curve",
        ],
        **kwargs
    )

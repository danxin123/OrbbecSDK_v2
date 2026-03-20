# Migration from Orbbec SDK v1 to Orbbec SDK v2

Orbbec SDK has been open-sourced since v2.0.0 to provide better performance, broader ecosystem support, and long-term maintainability.

## Upgrade Recommendation

For projects currently using Orbbec SDK v1.x and planning to migrate to v2.x, we recommend starting with pre-compiled binaries from:

- [OrbbecSDK_v2 Releases](https://github.com/orbbec/OrbbecSDK_v2/releases)

This helps reduce migration cost and keeps runtime behavior aligned with official release builds.

## API Compatibility

Orbbec SDK v2 keeps compatibility for most common usage patterns from v1.

- Common workflows and core capability paths are preserved.
- Some low-usage or legacy interfaces were adjusted or removed for long-term architecture improvements.

## Migration Resources

Use the following documents for a step-by-step migration:

- Main migration guide: [orbbecsdkv1_to_openorbbecsdkv2.md](orbbecsdkv1_to_openorbbecsdkv2.md)
- API change list: [../api/api_changes_at_v2.x.x.md](../api/api_changes_at_v2.x.x.md)

## Device Support Policy (v1 vs v2)

The table below summarizes support status differences between Orbbec SDK v1 and Orbbec SDK v2.

| Product Series | Product | Orbbec SDK v1 | Orbbec SDK v2 |
|---|---|---|---|
| Gemini 305 | Gemini 305 | not supported | recommended for new designs |
| Gemini 340 | Gemini 345 | not supported | recommended for new designs |
| Gemini 340 | Gemini 345Lg | not supported | recommended for new designs |
| Gemini 435Le | Gemini 435Le | not supported | recommended for new designs |
| Gemini 330 | Gemini 335Le | not supported | recommended for new designs |
| Gemini 330 | Gemini 335 | full maintenance | recommended for new designs |
| Gemini 330 | Gemini 336 | full maintenance | recommended for new designs |
| Gemini 330 | Gemini 330 | full maintenance | recommended for new designs |
| Gemini 330 | Gemini 335L | full maintenance | recommended for new designs |
| Gemini 330 | Gemini 336L | full maintenance | recommended for new designs |
| Gemini 330 | Gemini 330L | full maintenance | recommended for new designs |
| Gemini 330 | Gemini 335Lg | not supported | recommended for new designs |
| Gemini 2 | Gemini 2 | full maintenance | recommended for new designs |
| Gemini 2 | Gemini 2 L | full maintenance | recommended for new designs |
| Gemini 2 | Gemini 2 XL | recommended for new designs | to be supported |
| Gemini 2 | Gemini 215 | not supported | recommended for new designs |
| Gemini 2 | Gemini 210 | not supported | recommended for new designs |
| Femto | Femto Bolt | full maintenance | recommended for new designs |
| Femto | Femto Mega | full maintenance | recommended for new designs |
| Femto | Femto Mega I | full maintenance | recommended for new designs |
| Astra | Astra 2 | full maintenance | recommended for new designs |
| Astra | Astra+ | limited maintenance | not supported |
| Astra | Astra Pro Plus | limited maintenance | not supported |
| Astra Mini | Astra Mini Pro | full maintenance | recommended for new designs |
| Astra Mini | Astra Mini S Pro | full maintenance | recommended for new designs |
| LiDAR | Pulsar ME450 | not supported | recommended for new designs |
| LiDAR | Pulsar SL450 | not supported | recommended for new designs |

Support definitions:

1. recommended for new designs: full support with new features, bug fixes, and performance optimization
2. full maintenance: bug-fix support
3. limited maintenance: critical bug-fix support
4. not supported: no support in this SDK version
5. to be supported: planned support in future releases

## Continue Using v1

If your project must remain on v1.x for compatibility reasons, use:

- [Orbbec SDK v1](https://github.com/orbbec/OrbbecSDK)

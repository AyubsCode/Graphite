import { buttonVariants } from "@/components/ui/button";
import { page_routes } from "@/lib/routes-config";
import Link from "next/link";

export const metadata = {
  title: "Lucidity - Tutoring Scheduling Application",
  description: "A robust and user-friendly tutoring scheduling application that streamlines the process of connecting students with tutors.",
};

export default function Home() {
  return (
    <div className="flex sm:min-h-[91vh] min-h-[88vh] flex-col items-center justify-center text-center px-2 py-8">
      <h1 className="text-5xl font-bold mb-4 sm:text-7xl">
        Lucidity
      </h1>
      <h1 className="text-3xl font-bold mb-4 sm:text-5xl">
        The ins and outs of our design
      </h1>
      <p className="mb-8 sm:text-md max-w-[800px] text-muted-foreground">
        A robust and user-friendly tutoring scheduling application that streamlines the process of connecting students with tutors.
      </p>
      <div>
        <Link
          href={`/docs${page_routes[0].href}`}
          className={buttonVariants({
            className: "px-6 !font-medium",
            size: "lg",
          })}
        >
          Learn More
        </Link>
      </div>
    </div>
  );
}

